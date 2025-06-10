#include "network/driver/bridge_network_driver.h"
#include "easylogging++.h"
#include "network/driver/network_driver.h"
#include "network/ipam.h"
#include "network/network.h"
#include "network/rtnl/ip_rtnl_manager.h"
#include <string>

namespace zcontainer {

Network BridgeNetworkDriver::Create(const std::string &name,
                                    const std::string &subnet) {

  rtnl_manager_->CreateBridge(name);

  // 分配ip
  IPAM &ipam = GetIPAM();
  std::string ip = ipam.Allocate(subnet);
  std::string mask = subnet.substr(subnet.find('/') + 1);
  rtnl_manager_->SetLinkIP(name, ip + "/" + mask);

  rtnl_manager_->SetLinkUp(name);
  Network network(name, IPNet(ip + "/" + mask), "bridge");

  std::string ip_tables_cmd = "iptables -t nat -A POSTROUTING -s " + subnet +
                              " ! -o " + name + " -j MASQUERADE";
  system(ip_tables_cmd.c_str());

  return network;
}

void BridgeNetworkDriver::Delete(const std::string &name) {
  Network network;
  network.Load(name);
  rtnl_manager_->DeleteLink(name);
  std::string ip_tables_cmd = "iptables -t nat -D POSTROUTING -s " +
                              network.ip_range_.GetSubnet() + " ! -o " + name +
                              " -j MASQUERADE";
  system(ip_tables_cmd.c_str());
  remove((Network::NETWORK_DIR_PATH + name + ".json").c_str());
}

void BridgeNetworkDriver::Connect(const std::string &name, Endpoint &endpoint) {
  rtnl_manager_->CreateVethPair(endpoint.veth_.GetName(),
                                endpoint.veth_.GetPeerName());
  // 一端连网桥
  rtnl_manager_->SetLinkUp(endpoint.veth_.GetName());
  rtnl_manager_->AddLinkToBridge(endpoint.veth_.GetName(), name);

  // name2,peer一端连容器
  Network network;
  network.Load(name);
  IPAM &ipam = GetIPAM();
  std::string alloc_ip = ipam.Allocate(network.ip_range_);
  std::string alloc_ip_net =
      alloc_ip + "/" + std::to_string(network.ip_range_.mask_size_);
  LOG(INFO) << "container@" << endpoint.id_ << " alloc ip: " << alloc_ip_net
            << '\n';

  rtnl_manager_->SetLinkPidNamespace(endpoint.veth_.GetPeerName(),
                                     endpoint.container_pid_);
  // 开启lo
  rtnl_manager_->SetLinkUpNamespace("lo", endpoint.id_);
  rtnl_manager_->SetLinkIpNamespace(endpoint.veth_.GetPeerName(), alloc_ip_net,
                                    endpoint.id_);
  rtnl_manager_->SetLinkUpNamespace(endpoint.veth_.GetPeerName(), endpoint.id_);
  rtnl_manager_->AddDefaultRouteNamespace(
      endpoint.veth_.GetPeerName(), network.ip_range_.GetIP(), endpoint.id_);
  endpoint.ip_addr_ = alloc_ip;

  // 处理port映射
  for (auto &port_mapping : endpoint.port_mappings_) {
    int pos = port_mapping.find(':');
    std::string host_port = port_mapping.substr(0, pos);
    std::string container_port = port_mapping.substr(pos + 1);
    std::string cmd = "iptables -t nat -A PREROUTING ! -i " + name +
                      " -p tcp -m tcp --dport " + host_port +
                      " -j DNAT --to-destination " + alloc_ip + ":" +
                      container_port;
    system(cmd.c_str());

    // output chain DNAT
    cmd = "iptables -t nat -A OUTPUT -p tcp -m tcp --dport " + host_port +
          " -j DNAT --to-destination " + alloc_ip + ":" + container_port;
    system(cmd.c_str());
  }
}

void BridgeNetworkDriver::Disconnect(const std::string &name,
                                     const Endpoint &endpoint) {
  // rtnl_manager_->DelLinkFromBridge(endpoint.veth_.GetName(), name);
  // rtnl_manager_->DeleteLink(endpoint.veth_.GetName());

  // 删除port映射
  for (auto &port_mapping : endpoint.port_mappings_) {
    int pos = port_mapping.find(':');
    std::string host_port = port_mapping.substr(0, pos);
    std::string container_port = port_mapping.substr(pos + 1);
    std::string cmd = "iptables -t nat -D PREROUTING ! -i " + name +
                      " -p tcp -m tcp --dport " + host_port +
                      " -j DNAT --to-destination " + endpoint.ip_addr_ + ":" +
                      container_port;
    system(cmd.c_str());

    // output chain DNAT
    cmd = "iptables -t nat -D OUTPUT -p tcp -m tcp --dport " + host_port +
          " -j DNAT --to-destination " + endpoint.ip_addr_ + ":" +
          container_port;
    system(cmd.c_str());
  }
}

} // namespace zcontainer