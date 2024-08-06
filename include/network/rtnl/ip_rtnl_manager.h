#ifndef NETWORK_RTNL_IP_RTNL_MANAGER_H_
#define NETWORK_RTNL_IP_RTNL_MANAGER_H_

#include "network/rtnl/rtnl_manager.h"

namespace zcontainer {
class IPRTNLManager: public RTNLManager {
public:
	IPRTNLManager() = default;
	IPRTNLManager(const IPRTNLManager &) = default;
	IPRTNLManager &operator=(const IPRTNLManager &) = default;
	IPRTNLManager(IPRTNLManager &&) = default;
	IPRTNLManager &operator=(IPRTNLManager &&) = default;
	~IPRTNLManager() = default;

	void CreateBridge(const std::string &name) override;
	void CreateVethPair(const std::string &name1,
											const std::string &name2) override;
	void AddLinkToBridge(const std::string &link,
											const std::string &bridge) override;
	void DelLinkFromBridge(const std::string &link,
											const std::string &bridge) override;
	void SetLinkUp(const std::string &link) override;
	void DeleteLink(const std::string &link) override;
	void SetLinkIP(const std::string &link, const std::string &ip_net) override;
	void SetLinkPidNamespace(const std::string &link, int pid) override;
	void SetLinkUpNamespace(const std::string &link, const std::string &ns) override;
	void SetLinkIpNamespace(const std::string &link, const std::string &ip_net, const std::string &ns) override;
	void AddDefaultRouteNamespace(const std::string &link,const std::string &route_ip,const std::string &ns) override;
};
}

#endif // NETWORK_RTNL_IP_RTNL_MANAGER_H_