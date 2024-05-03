#ifndef CONTAINER_CONTAINER_H
#define CONTAINER_CONTAINER_H

#include "cgroups/cgroups_manager.h"
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

  struct RunParams {
    bool tty{false};
    std::vector<std::string> cmds{};
    std::string mem{};
    std::string cpu{};
    std::string cpuset{};
    int uid;
    int gid;
  };

  int RunContainer(const RunParams &params);

private:
  CgroupsManager cgroups_manager_;
};
}; // namespace zcontainer

#endif