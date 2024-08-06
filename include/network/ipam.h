#ifndef NETWORK_IPAM_H
#define NETWORK_IPAM_H

#include "json.hpp"
#include "network/ip_net.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace zcontainer {
class IPAM {

public:
  IPAM();
  ~IPAM() = default;
  IPAM(const IPAM &) = delete;
  IPAM &operator=(const IPAM &) = delete;
  IPAM(IPAM &&) = default;
  IPAM &operator=(IPAM &&) = default;

  static const std::string IPAM_FILE_PATH;

  // Allocate a new IP address
  std::string Allocate(const IPNet &ip_net);

  // Release an IP address
  void Release(const IPNet &ip_net, const std::string &ip);

  bool HasAllocated(const IPNet &ip_net, const std::string &ip);

  friend void to_json(nlohmann::json &j, const IPAM &ipam);

  friend void from_json(const nlohmann::json &j, IPAM &ipam);

private:
	// IPNet -> IP bitset
  std::unordered_map<std::string, std::string> ip_map_;

  bool Load();

  void Save();
};
}; // namespace zcontainer

#endif
