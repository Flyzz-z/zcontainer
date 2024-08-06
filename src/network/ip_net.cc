#include "network/ip_net.h"
#include "easylogging++.h"
#include "json.hpp"
#include <netinet/in.h>

namespace zcontainer {

IPNet::IPNet(std::string ip_net) {
  std::string ip_str = ip_net.substr(0, ip_net.find('/'));
  std::string mask_str = ip_net.substr(ip_net.find('/') + 1);
  inet_aton(ip_str.c_str(), &ip_);
  mask_.s_addr = htonl(~((1 << (32 - std::stoi(mask_str))) - 1));
  mask_size_ = std::stoi(mask_str);
}

bool IPNet::Contains(const std::string &ip_str) const {
  in_addr ip_addr;
  inet_aton(ip_str.c_str(), &ip_addr);
  return (ip_addr.s_addr & mask_.s_addr) == (ip_.s_addr & mask_.s_addr);
}

int IPNet::MaskSize() const { return mask_size_; }

int IPNet::Size() const { return 1 << (32 - MaskSize()); }

std::string IPNet::GetNetwork() const {
  in_addr net_addr;
  net_addr.s_addr = ip_.s_addr & mask_.s_addr;
  return inet_ntoa(net_addr);
}

std::string IPNet::GetIP() const { return inet_ntoa(ip_); }

std::string IPNet::GetSubnet() const {
	return GetIP() + "/" + std::to_string(mask_size_);
}
}; // namespace zcontainer