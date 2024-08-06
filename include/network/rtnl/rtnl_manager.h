#ifndef NETWORK_RTNL_RTNL_MANAGER_H
#define NETWORK_RTNL_RTNL_MANAGER_H

#include <string>

namespace zcontainer {

class RTNLManager {
public:
  virtual ~RTNLManager() {}

  virtual void CreateBridge(const std::string &name) = 0;

  virtual void CreateVethPair(const std::string &name1,
                              const std::string &name2) = 0;

  virtual void AddLinkToBridge(const std::string &link,
                               const std::string &bridge) = 0;

  virtual void DelLinkFromBridge(const std::string &link,
                                 const std::string &bridge) = 0;

  virtual void SetLinkUp(const std::string &link) = 0;

  virtual void DeleteLink(const std::string &link) = 0;

  virtual void SetLinkIP(const std::string &link, const std::string &ip) = 0;

  virtual void SetLinkPidNamespace(const std::string &link, int pid) = 0;

  virtual void SetLinkUpNamespace(const std::string &link, const std::string &ns) = 0;

	virtual void SetLinkIpNamespace(const std::string &link, const std::string &ip_net, const std::string &ns) = 0;

	virtual void AddDefaultRouteNamespace(const std::string &link,const std::string &route_ip,const std::string &ns) = 0;
};
} // namespace zcontainer

#endif // NETWORK_RTNL_UTIL_H