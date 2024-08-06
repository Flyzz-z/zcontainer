#ifndef NETWORK_RTNL_LIBNL_RTNL_MANAGER_H
#define NETWORK_RTNL_LIBNL_RTNL_MANAGER_H

#include "network/rtnl/rtnl_manager.h"
#include <libnl3/netlink/handlers.h>
#include <libnl3/netlink/netlink.h>

namespace zcontainer {

class LibnlRTNLManager : public RTNLManager {
public:
  LibnlRTNLManager();
	LibnlRTNLManager(const LibnlRTNLManager &) = delete;
	LibnlRTNLManager &operator=(const LibnlRTNLManager &) = delete;
	LibnlRTNLManager(LibnlRTNLManager &&);
	LibnlRTNLManager &operator=(LibnlRTNLManager &&);
  ~LibnlRTNLManager();

  void CreateBridge(const std::string &name) override;
  void CreateVethPair(const std::string &name1,
                      const std::string &name2) override;
  void AddLinkToBridge(const std::string &link,
                       const std::string &bridge) override;
  void SetLinkUp(const std::string &link) override;
  void DeleteLink(const std::string &link) override;
  void SetLinkIP(const std::string &link, const std::string &ip) override;

private:
	nl_sock *nl_sock_;
};
} // namespace zcontainer

#endif // NETWORK_RTNL_LIBNL_RTNL_MANAGER_H