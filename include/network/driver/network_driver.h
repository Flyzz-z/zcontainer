#ifndef NETWORK_DRIVER_NETWORK_DRIVER_H
#define NETWORK_DRIVER_NETWORK_DRIVER_H

#include "network/driver/endpoint.h"
#include "network/ipam.h"
#include "network/network.h"
#include <string>

namespace zcontainer {
class NetworkDriver {
public:
  NetworkDriver() = delete;
	NetworkDriver(const std::string &name) : name_(name) {}
  virtual ~NetworkDriver() = default;
	NetworkDriver(const NetworkDriver &) = delete;
	NetworkDriver &operator=(const NetworkDriver &) = delete;
	NetworkDriver(NetworkDriver &&) = default;
	NetworkDriver &operator=(NetworkDriver &&) = default;

	virtual Network Create(const std::string &name, const std::string &subnet) = 0;
	virtual void Delete(const std::string &name) = 0;
	virtual void Connect(const std::string &name, Endpoint &endpoint) = 0;
	virtual void Disconnect(const std::string &name, const Endpoint &endpoint) = 0;
	std::string GetName() const { return name_; }
	IPAM &GetIPAM() { return ipam_; }

private:
  std::string name_;
	IPAM ipam_;
};

} // namespace zcontainer

#endif // NETWORK_DRIVER_NETWORK_DRIVER_H