#include "network/network_manager.h"
#include "easylogging++.h"
#include "network/driver/bridge_network_driver.h"
#include "network/driver/endpoint.h"
#include "network/network.h"
#include "utils.h"
#include <cstdio>
#include <dirent.h> // 包含目录操作的头文件
#include <memory>

namespace zcontainer {

NetworkManager::NetworkManager() {
  drivers_.push_back(std::make_unique<BridgeNetworkDriver>());
}

Network NetworkManager::CreateNetwork(const std::string &name,
                                      const std::string &driver,
                                      const std::string &subnet) {
  Network network(name, IPNet(subnet), driver);
  // 创建网桥
  for (auto &network_driver : drivers_) {
    if (network_driver->GetName() == driver) {
      network = network_driver->Create(name, subnet);
      break;
    }
  }

  // 保存网络信息
  network.Dump();
  return network;
}

void NetworkManager::DeleteNetwork(const std::string &name) {

  Network network;
  network.Load(name);

  for (auto &network_driver : drivers_) {
    if (network.driver_ == network_driver->GetName()) {
      network_driver->Delete(name);
      break;
    }
  }

  std::string file_path = Network::NETWORK_DIR_PATH + name + ".json";
  remove(file_path.c_str());
}

std::vector<Network> NetworkManager::ListNetworks() const {
  std::vector<Network> networks;
  // 读取网络保存目录中所有json，加载网络信息
  DIR *dir;             // 声明DIR类型指针
  struct dirent *entry; // 声明dirent结构体指针

  dir = opendir(Network::NETWORK_DIR_PATH.c_str());
  if (dir == NULL) {
    LOG(ERROR) << "open network dir failed" << strerror(errno);
    return networks;
  }

  while ((entry = readdir(dir)) != NULL) {
    Network network;
    if (entry->d_type == DT_REG) {
      std::string network_name = entry->d_name;
      network_name = network_name.substr(0, network_name.find(".json"));
      network.Load(network_name);
      networks.push_back(network);
    }
  }
  return networks;
}

Endpoint NetworkManager::ConnectToNetwork(Endpoint &endpoint,
                                          const std::string &network_name) {

  endpoint.veth_.name1 = "veth" + endpoint.GetIfId();
  endpoint.veth_.name2 = "if" + endpoint.GetIfId();

  Network network;
  network.Load(network_name);
  endpoint.network_ = network;

  for (auto &network_driver : drivers_) {
    if (network_driver->GetName() == network.driver_) {
      network_driver->Connect(network_name, endpoint);
      break;
    }
  }

  return endpoint;
}

void NetworkManager::DisconnectFromNetwork(Endpoint &endpoint) {
  for (auto &network_driver : drivers_) {
    if (network_driver->GetName() == endpoint.network_.driver_) {
      network_driver->Disconnect(endpoint.network_.name_, endpoint);
      break;
    }
  }
}
} // namespace zcontainer