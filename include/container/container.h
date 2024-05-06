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

	static const std::string CONTAINER_STORAGE_PATH;

  struct RunParams {
		std::string container_id;
    bool tty{false};
    std::vector<std::string> cmds{};
    std::string mem{};
    std::string cpu{};
    std::string cpuset{};
    int uid;
    int gid;
  };

  int RunContainer(RunParams &params);

private:
  CgroupsManager cgroups_manager_;
	std::string container_id_;
};
}; // namespace zcontainer

#endif