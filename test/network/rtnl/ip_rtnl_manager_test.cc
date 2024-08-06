#include "easylogging++.h"
#include "network/rtnl/ip_rtnl_manager.h"
#include "gtest/gtest.h"

INITIALIZE_EASYLOGGINGPP

namespace zcontainer {

TEST(IPRTNLManagerTest, CreateBridgeTest) {
  IPRTNLManager ip_rtnl_manager;
  ip_rtnl_manager.CreateBridge("br0");
	ip_rtnl_manager.DeleteLink("br0");
}

TEST(IPRTNLManagerTest, AddLinkToBridgeTest) {
	IPRTNLManager ip_rtnl_manager;
	ip_rtnl_manager.CreateBridge("br0");
	ip_rtnl_manager.SetLinkIP("br0", "172.24.0.0/13");
	ip_rtnl_manager.SetLinkUp("br0");
	ip_rtnl_manager.CreateVethPair("veth0", "veth1");
	ip_rtnl_manager.AddLinkToBridge("veth0", "br0");
	ip_rtnl_manager.SetLinkUp("veth0");

	ip_rtnl_manager.DeleteLink("veth0");
	ip_rtnl_manager.DeleteLink("br0");
}
} // namespace zcontainer