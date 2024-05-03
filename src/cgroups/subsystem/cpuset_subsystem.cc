#include "cgroups/subsystem/cpuset_subsystem.h"
#include "easylogging++.h"

namespace zcontainer {
CpusetSubsystem::CpusetSubsystem()
    : Subsystem("cpuset", SubsystemType::kCpuset) {}

bool CpusetSubsystem::SetResourceLimit(const std::string &cgroup,
                                       const ResourceConfig &res) {
  if (res.cpuset_cpus.empty()) {
    LOG(INFO) << "cpuset_cpus is empty";
    return false;
  }

  std::string file_path =
      MountPrefix() + "/" + Name() + "/" + cgroup + "/cpuset.cpus";
  std::ofstream file(file_path);
  if (!file) {
    LOG(ERROR) << "open file " << file_path << " failed";
    return false;
  }
  file << res.cpuset_cpus;
  file.close();
	return true;
}
}; // namespace zcontainer