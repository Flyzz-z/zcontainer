#ifndef NETWORK_NETWORK_MANAGER_H
#define NETWORK_NETWORK_MANAGER_H

#include "network/driver/endpoint.h"
#include "network/driver/network_driver.h"
#include "network/ipam.h"
#include "network/network.h"
#include "network/rtnl/ip_rtnl_manager.h"
#include "network/rtnl/rtnl_manager.h"
#include <memory>
#include <vector>

namespace zcontainer {

class NetworkManager {
public:
  NetworkManager();
  NetworkManager(const NetworkManager &) = delete;
  NetworkManager &operator=(const NetworkManager &) = delete;
  NetworkManager(NetworkManager &&) = default;
  NetworkManager &operator=(NetworkManager &&) = default;
  ~NetworkManager() = default;

  Network CreateNetwork(const std::string &name, const std::string &driver,
                        const std::string &subnet);

  void DeleteNetwork(const std::string &name);

  std::vector<Network> ListNetworks() const;

  Endpoint ConnectToNetwork(Endpoint &endpoint, const std::string &network_name);

	void DisconnectFromNetwork(Endpoint &endpoint);

private:
  std::vector<std::unique_ptr<NetworkDriver>> drivers_;
};
} // namespace zcontainer

#endif // NETWORK_NETWORK_MANAGER_H