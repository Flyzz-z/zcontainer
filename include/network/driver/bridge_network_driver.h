#ifndef NETWORK_DRIVER_BRIDGE_NETWORK_DRIVER_H
#define NETWORK_DRIVER_BRIDGE_NETWORK_DRIVER_H

#include "network/driver/endpoint.h"
#include "network/driver/network_driver.h"
#include "network/network.h"
#include "network/rtnl/ip_rtnl_manager.h"
#include "network/rtnl/rtnl_manager.h"
#include <memory>

namespace zcontainer {

class BridgeNetworkDriver : public NetworkDriver {
public:
  BridgeNetworkDriver()
      : NetworkDriver("bridge"),
        rtnl_manager_(std::make_unique<IPRTNLManager>()){};
  BridgeNetworkDriver(std::unique_ptr<RTNLManager> &&rtnl_manager)
      : NetworkDriver("bridge"), rtnl_manager_(std::move(rtnl_manager)) {}
  ~BridgeNetworkDriver() = default;
  BridgeNetworkDriver(const BridgeNetworkDriver &) = delete;
  BridgeNetworkDriver &operator=(const BridgeNetworkDriver &) = delete;
  BridgeNetworkDriver(BridgeNetworkDriver &&) = default;
  BridgeNetworkDriver &operator=(BridgeNetworkDriver &&) = default;

  Network Create(const std::string &name, const std::string &subnet) override;
  void Delete(const std::string &name) override;
  void Connect(const std::string &name, Endpoint &endpoint) override;
  void Disconnect(const std::string &name, const Endpoint &endpoint) override;

private:
  std::unique_ptr<RTNLManager> rtnl_manager_;
};
} // namespace zcontainer

#endif // NETWORK_DRIVER_BRIDGE_NETWORK_DRIVER_H