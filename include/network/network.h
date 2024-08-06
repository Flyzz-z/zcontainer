#ifndef NETWORK_NETWORK_H
#define NETWORK_NETWORK_H

#include "network/ip_net.h"
#include <arpa/inet.h>
#include <iostream>
#include <string>

namespace zcontainer {
class Network {
public:
  Network() = default;
  Network(const std::string &name, const IPNet &ip_range,
          const std::string &driver)
      : name_(name), ip_range_(ip_range), driver_(driver) {}
  ~Network() = default;
  Network(const Network &) = default;
  Network &operator=(const Network &) = default;
  Network(Network &&) = default;
  Network &operator=(Network &&) = default;

  static const std::string NETWORK_DIR_PATH;

  std::string name_;
  IPNet ip_range_;
  std::string driver_;

  void Dump() const;

  void Load(const std::string &name);

  void Print() const;
};
} // namespace zcontainer

#endif // NETWORK_NETWORK_H