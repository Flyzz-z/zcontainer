#ifndef CONTAINER_CONTAINER_H
#define CONTAINER_CONTAINER_H

#include <cstddef>
#include <iostream>
#include <sched.h>
#include <string>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include "../easylogging++.h"

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
		int uid;
		int gid;
	};


  int RunContainer(const RunParams &params);
};
}; // namespace zcontainer

#endif