#include "network/network.h"
#include "json.hpp"
#include <fstream>

namespace zcontainer {

void to_json(nlohmann::json &j, const Network &network) {
  j = nlohmann::json{};
  j["name"] = network.name_;
  j["ip_range"]["ip"] = inet_ntoa(network.ip_range_.ip_);
  j["ip_range"]["mask"] = inet_ntoa(network.ip_range_.mask_);
  j["ip_range"]["mask_size"] = network.ip_range_.mask_size_;
  j["driver"] = network.driver_;
}

void from_json(const nlohmann::json &j, Network &network) {
  network.name_ = j.at("name").get<std::string>();
  network.ip_range_.ip_.s_addr =
      inet_addr(j.at("ip_range").at("ip").get<std::string>().c_str());
  network.ip_range_.mask_.s_addr =
      inet_addr(j.at("ip_range").at("mask").get<std::string>().c_str());
  network.ip_range_.mask_size_ = j.at("ip_range").at("mask_size").get<int>();
  network.driver_ = j.at("driver").get<std::string>();
}

const std::string Network::NETWORK_DIR_PATH =
    "/var/lib/zcontainer/network/networks/";

void Network::Dump() const {
  nlohmann::json j = *this;
  std::string file_path = NETWORK_DIR_PATH + name_ + ".json";
  std::ofstream file(file_path);
  file << j.dump(4);
  file.close();
}

void Network::Load(const std::string &name) {
  std::string file_path = NETWORK_DIR_PATH + name + ".json";
  std::ifstream file(file_path);
  nlohmann::json j;
  file >> j;
  *this = j.get<Network>();
}

void Network::Print() const {
  std::cout << "name: " << name_ << std::endl;
  std::cout << "ip_range: " << inet_ntoa(ip_range_.ip_) << "/"
            << ip_range_.mask_size_ << std::endl;
  std::cout << "driver: " << driver_ << std::endl;
}
} // namespace zcontainer