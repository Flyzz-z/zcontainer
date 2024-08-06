#ifndef NETWORK_IP_NET_H
#define NETWORK_IP_NET_H

#include<string>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace zcontainer {
class IPNet {
public:

	IPNet() = default;
	IPNet(std::string ip_net);

	IPNet(const IPNet& ip_net) = default;
	IPNet& operator=(const IPNet& ip_net) = default;
	IPNet(IPNet&& ip_net) = default;
	IPNet& operator=(IPNet&& ip_net) = default;

	bool Contains(const std::string& ip_str) const;

	int MaskSize() const;

	int Size() const;

	//get network address
	std::string GetNetwork() const;

	std::string GetIP() const;

	std::string GetSubnet() const;

	in_addr ip_;
	in_addr mask_;
	int mask_size_;
};
}; // namespace zcontainer

#endif