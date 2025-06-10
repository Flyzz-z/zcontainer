#include "network/rtnl/libnl_rtnl_manager.h"
#include "easylogging++.h"
#include <libnl3/netlink/object.h>
#include <libnl3/netlink/route/link.h>
#include <libnl3/netlink/socket.h>
#include <net/if.h>

namespace zcontainer {

LibnlRTNLManager::LibnlRTNLManager() {
  nl_sock_ = nl_socket_alloc();
  if (!nl_sock_) {
    LOG(ERROR) << "Failed to allocate netlink socket";
    return;
  }

  if (nl_connect(nl_sock_, NETLINK_ROUTE) != 0) {
    LOG(ERROR) << "Failed to connect to netlink socket";
    return;
  }
}

LibnlRTNLManager::LibnlRTNLManager(LibnlRTNLManager &&other) {
  nl_sock_ = other.nl_sock_;
  other.nl_sock_ = nullptr;
}

LibnlRTNLManager &LibnlRTNLManager::operator=(LibnlRTNLManager &&other) {
  if (this != &other) {
    nl_sock_ = other.nl_sock_;
    other.nl_sock_ = nullptr;
  }
  return *this;
}

LibnlRTNLManager::~LibnlRTNLManager() {
  if (nl_sock_) {
    nl_close(nl_sock_);
    nl_socket_free(nl_sock_);
  }
}

void LibnlRTNLManager::CreateBridge(const std::string &name) {
  struct rtnl_link *link = rtnl_link_alloc();
  if (!link) {
    LOG(ERROR) << "Failed to allocate link";
    return;
  }

  rtnl_link_set_name(link, name.c_str());

  if (rtnl_link_set_type(link, "bridge") != 0) {
    LOG(ERROR) << "Failed to set link type";
    rtnl_link_put(link);
    return;
  }

  if (rtnl_link_add(nl_sock_, link, NLM_F_CREATE | NLM_F_EXCL) != 0) {
    LOG(ERROR) << "Failed to add link";
    rtnl_link_put(link);
    return;
  }

  rtnl_link_put(link);
}

void LibnlRTNLManager::CreateVethPair(const std::string &name1,
                                      const std::string &name2) {}

void LibnlRTNLManager::AddLinkToBridge(const std::string &link,
                                       const std::string &bridge) {
  struct rtnl_link *link_link = nullptr;
  struct rtnl_link *link_bridge = nullptr;
  struct nl_cache *link_cache = nullptr;

  // 获取网络接口缓存
  if (rtnl_link_alloc_cache(nl_sock_, AF_UNSPEC, &link_cache) < 0) {
    throw std::runtime_error("Failed to allocate link cache.");
  }

  link_link = rtnl_link_get_by_name(link_cache, link.c_str());
  if (!link_link) {
    throw std::runtime_error("Failed to get link by name.");
  }

  link_bridge = rtnl_link_get_by_name(link_cache, bridge.c_str());
  if (!link_bridge) {
    throw std::runtime_error("Failed to get link by name.");
  }

  rtnl_link_set_master(link_link, rtnl_link_get_ifindex(link_bridge));

  rtnl_link_put(link_link);
  rtnl_link_put(link_bridge);
  nl_cache_free(link_cache);
}

void LibnlRTNLManager::SetLinkUp(const std::string &link) {
  struct rtnl_link *link_link = nullptr;
  struct nl_cache *link_cache = nullptr;

  if (rtnl_link_alloc_cache(nl_sock_, AF_UNSPEC, &link_cache) < 0) {
    throw std::runtime_error("Failed to allocate link cache.");
  }

  link_link = rtnl_link_get_by_name(link_cache, link.c_str());
  if (!link_link) {
    throw std::runtime_error("Failed to get link by name.");
  }

  rtnl_link_set_flags(link_link, IFF_UP);

  rtnl_link_put(link_link);
  nl_cache_free(link_cache);
}
} // namespace zcontainer