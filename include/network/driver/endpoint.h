#ifndef NETWORK_DRIVER_ENDPOINT_H
#define NETWORK_DRIVER_ENDPOINT_H

#include "network/network.h"
#include "network/veth.h"
#include <string>
#include <vector>

namespace zcontainer {

class Endpoint {
public:
		std::string id_;
		int container_pid_;
		Veth veth_;
		std::string ip_addr_;
		std::string mac_addr_;
		Network network_;
		std::vector<std::string> port_mappings_;

		std::string GetIfId() const {
			return id_.substr(0, 3) + std::to_string(container_pid_);
		}
};

};

#endif