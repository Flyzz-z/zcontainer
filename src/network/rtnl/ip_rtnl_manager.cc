#include "network/rtnl/ip_rtnl_manager.h"
#include "easylogging++.h"
#include <cstdlib>
#include <stdexcept>
#include <string>

namespace zcontainer {
void IPRTNLManager::CreateBridge(const std::string &name) {
  std::string cmd = "ip link add name " + name + " type bridge";
  int ret = system(cmd.c_str());
  if (ret != 0) {
    throw std::runtime_error("Failed to create bridge");
  }
}

void IPRTNLManager::CreateVethPair(const std::string &name1,
                                   const std::string &name2) {
  std::string cmd =
      "ip link add name " + name1 + " type veth peer name " + name2;
  int ret = system(cmd.c_str());
  if (ret != 0) {
    throw std::runtime_error("Failed to create veth pair");
  }
}

void IPRTNLManager::AddLinkToBridge(const std::string &link,
                                    const std::string &bridge) {
  std::string cmd = "ip link set " + link + " master " + bridge;
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to add link to bridge");
  }
}

void IPRTNLManager::DelLinkFromBridge(const std::string &link,
                                      const std::string &bridge) {
  std::string cmd = "ip link set " + link + " nomaster";
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to delete link from bridge");
  }
}

void IPRTNLManager::SetLinkUp(const std::string &link) {
  std::string cmd = "ip link set " + link + " up";
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to set link up");
  }
}

void IPRTNLManager::DeleteLink(const std::string &link) {
  std::string cmd = "ip link delete " + link;
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to delete link");
  }
}

void IPRTNLManager::SetLinkIP(const std::string &link,
                              const std::string &ip_net) {
  std::string cmd = "ip addr add " + ip_net + " dev " + link;
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to set link IP");
  }
}

void IPRTNLManager::SetLinkPidNamespace(const std::string &link, int pid) {
  std::string cmd = "ip link set " + link + " netns " + std::to_string(pid);
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to set link pid namespace");
  }
}

void IPRTNLManager::SetLinkUpNamespace(const std::string &link, const std::string &ns) {
  std::string cmd =
      "ip netns exec " + ns + " ip link set " + link + " up";
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to set link up in pid namespace");
  }
}

void IPRTNLManager::SetLinkIpNamespace(const std::string &link,
                                          const std::string &ip_net, const std::string &ns) {
  std::string cmd = "ip netns exec " + ns + " ip addr add " +
                    ip_net + " dev " + link;
  if (system(cmd.c_str()) < 0) {
    throw std::runtime_error("Failed to set link ip in pid namespace");
  }
}

void IPRTNLManager::AddDefaultRouteNamespace(const std::string &link,const std::string &route_ip,const std::string &ns) {
	std::string cmd = "ip netns exec " + ns + " ip route add default via " + route_ip + " dev " + link;
	if (system(cmd.c_str()) < 0) {
		throw std::runtime_error("Failed to add default route in pid namespace");
	}
}



} // namespace zcontainer