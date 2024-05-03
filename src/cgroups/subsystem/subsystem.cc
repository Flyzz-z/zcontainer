#include "cgroups/subsystem/subsystem.h"
#include "easylogging++.h"
#include <sys/stat.h>
#include <unistd.h>

namespace zcontainer {

bool Subsystem::Create(const std::string &cgroup) {
  std::string file_path = MountPrefix() + "/" + Name() + "/" + cgroup;
  if (mkdir(file_path.c_str(), 0755) == -1) {
    LOG(ERROR) << "create dir " << file_path << " failed" << strerror(errno);
    return false;
  }
  return true;
}

bool Subsystem::Apply(int pid, const std::string &cgroup) {
	std::string file_path = MountPrefix() + "/" + Name() + "/" + cgroup + "/tasks";
	std::ofstream file(file_path);
	if (!file) {
		LOG(ERROR) << "open file " << file_path << " failed" << strerror(errno);
		return false;
	}
	file << pid;
	file.close();
	return true;
}

void Subsystem::Remove(const std::string &cgroup) {
	std::string file_path = MountPrefix() + "/" + Name() + "/" + cgroup;
	rmdir(file_path.c_str());
}
}; // namespace zcontainer