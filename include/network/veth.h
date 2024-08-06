#ifndef NETWORK_VETH_H
#define NETWORK_VETH_H

#include "network/ip_net.h"
#include <string>

namespace zcontainer {
class Veth {
public:
  Veth() = default;
  Veth(const Veth &) = default;
  Veth &operator=(const Veth &) = default;
  Veth(Veth &&) = default;
  Veth &operator=(Veth &&) = default;
  ~Veth() = default;

  std::string GetName() const { return name1; }

  std::string GetPeerName() const { return name2; }

  std::string name1;
  std::string name2;
  IPNet ip_net1;
  IPNet ip_net2;
};
} // namespace zcontainer

#endif // NETWORK_VETH_H