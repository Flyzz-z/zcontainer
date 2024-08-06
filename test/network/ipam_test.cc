#include "easylogging++.h"
#include "network/ip_net.h"
#include "network/ipam.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

INITIALIZE_EASYLOGGINGPP

namespace zcontainer {

TEST(IPAMTest, BasicTest) {
  IPAM ipam;
  IPNet ip_net("192.168.5.0/24");
  std::string ip = ipam.Allocate(ip_net);
  EXPECT_EQ(ipam.HasAllocated(ip_net, ip), true);

  ipam.Release(ip_net, ip);
  EXPECT_EQ(ipam.HasAllocated(ip_net, ip), false);
}

TEST(IPAMTest, MultiAllocTest) {
  IPAM ipam;
  IPNet ip_net("192.168.5.0/24");
  std::vector<std::string> ips;
  for (int i = 0; i < 10; i++) {
    std::string ip = ipam.Allocate(ip_net);
    EXPECT_EQ(ipam.HasAllocated(ip_net, ip), true);
    ips.push_back(ip);
  }

  for (auto ip : ips) {
    ipam.Release(ip_net, ip);
    EXPECT_EQ(ipam.HasAllocated(ip_net, ip), false);
  }
}

} // namespace zcontainer