#ifndef CONTAINER_CONTAINER_H
#define CONTAINER_CONTAINER_H

#include "cgroups/cgroups_manager.h"
#include "network/network_manager.h"
#include <string>
#include <vector>

namespace zcontainer {
class Container {
public:
  Container() = default;
  ~Container() = default;
  Container(const Container &) = delete;
  Container &operator=(const Container &) = delete;
  Container(Container &&) = delete;
  Container &operator=(Container &&) = delete;

  static const std::string CONTAINER_STORAGE_PATH;

  struct RunParams {
    std::string container_id;
    bool tty{false};
    bool daemon{false};
    std::vector<std::string> cmds{};
    std::string mem{};
    std::string cpu{};
    std::string cpuset{};
    std::string network{};
    std::vector<std::string> volumes;
    std::vector<std::string> envs;
    std::vector<std::string> port_mappings;
  };

  struct ExecParams {
    std::string container_id;
    bool tty{false};
    std::vector<std::string> cmds;
  };

  int RunContainer(RunParams &params);
  bool ExecContainer(ExecParams &params);

private:
  CgroupsManager cgroups_manager_;
  NetworkManager network_manager_;
  std::string container_id_;
};
}; // namespace zcontainer

#endif