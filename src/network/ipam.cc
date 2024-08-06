#include "network/ipam.h"
#include "easylogging++.h"
#include "json.hpp"
#include "network/ip_net.h"
#include <arpa/inet.h>
#include <cerrno>
#include <fstream>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

namespace zcontainer {

void to_json(nlohmann::json &j, const IPAM &ipam) {
  j = nlohmann::json{{"ip_map", ipam.ip_map_}};
}

void from_json(const nlohmann::json &j, IPAM &ipam) {
  j.at("ip_map").get_to(ipam.ip_map_);
}

const std::string IPAM::IPAM_FILE_PATH =
    "/var/lib/zcontainer/network/ipam.json";

IPAM::IPAM() {
	if (access(IPAM_FILE_PATH.c_str(), F_OK) == -1) {
		std::ofstream ofs(IPAM_FILE_PATH);
		nlohmann::json j = *this;
		ofs << j.dump(4);
		ofs.close();
	}
}

std::string IPAM::Allocate(const IPNet &ip_net) {
  // 读取json
  if (!Load()) {
    return "";
  }
  // 对应网段不存在则创建
  std::string network = ip_net.GetNetwork();
  if (!ip_map_.count(network)) {
    std::string bitmap(ip_net.Size(), '0');
		// 主机地址全0保留用于网络地址，全1为广播地址
		bitmap[0] = '1';
		bitmap.back() = '1';
    ip_map_[ip_net.GetNetwork()] = bitmap;
  }
  // 分配一个ip地址，设置对应网段的ip地址已分配
  std::string alloc_ip = "";
  for (int i = 0; i < ip_net.Size(); i++) {
    if (ip_map_[network][i] == '0') {
      ip_map_[network][i] = '1';
      in_addr ip_addr;
      inet_aton(ip_net.GetNetwork().c_str(), &ip_addr);
      ip_addr.s_addr += htonl(i);
      alloc_ip = inet_ntoa(ip_addr);
			break; 
    }
  }

  // 写入json
  Save();
  return alloc_ip;
}

void IPAM::Release(const IPNet &ip_net, const std::string &ip) {
  // 读取json
  if (!Load()) {
    return;
  }
  // 对应网段不存在则返回
  std::string network = ip_net.GetNetwork();
  if (!ip_map_.count(network)) {
    return;
  }
  // 释放一个ip地址，设置对应网段的ip地址未分配
  in_addr ip_addr;
  inet_aton(ip.c_str(), &ip_addr);
  int index = ntohl(ip_addr.s_addr) - ntohl(ip_net.ip_.s_addr);
  if (index < 0 || index >= ip_net.Size()) {
    return;
  }

  ip_map_[network][index] = '0';
  // 写入json
  Save();
}

bool IPAM::Load() {
  // 读取json
  std::ifstream ifs(IPAM_FILE_PATH);
  if (!ifs.is_open()) {
		LOG(ERROR) << "open ipam file failed" << strerror(errno);
    return false;
  }

  nlohmann::json j;
  ifs >> j;
  j.template get_to(*this);
  ifs.close();

  return true;
}

void IPAM::Save() {
  // 写入json
  std::ofstream ofs(IPAM_FILE_PATH);
  if (!ofs.is_open()) {
    return;
  }

  nlohmann::json j = *this;
  ofs << j.dump(4);
  ofs.close();
}

bool IPAM::HasAllocated(const IPNet &ip_net, const std::string &ip) {
  // 读取json
  if (!Load()) {
    return false;
  }
  // 对应网段不存在则返回
  std::string network = ip_net.GetNetwork();
  if (!ip_map_.count(network)) {
    return false;
  }
  // 判断ip地址是否已分配
  in_addr ip_addr;
  inet_aton(ip.c_str(), &ip_addr);
  int index = ntohl(ip_addr.s_addr) - ntohl(ip_net.ip_.s_addr);
  if (index < 0 || index >= ip_net.Size()) {
    return false;
  }

  return ip_map_.at(network)[index] == '1';
}
}; // namespace zcontainer