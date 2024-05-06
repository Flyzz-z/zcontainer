

#include "cgroups/subsystem/cpu_subsystem.h"
#include "cgroups/subsystem/subsystem.h"
#include "easylogging++.h"
#include <cstring>
#include <fstream>
#include <unistd.h>

namespace zcontainer {

CpuSubsystem::CpuSubsystem() : Subsystem("cpu", SubsystemType::kCpu) {}

bool CpuSubsystem::SetResourceLimit(const std::string &cgroup,
                                    const ResourceConfig &res) {

  if (res.cpu_cfs_period_us.empty() || res.cpu_cfs_quota_us.empty()) {
    return false;
  }

  std::string file_path =
      MountPrefix() + "/" + Name() + "/" + cgroup + "/cpu.cfs_period_us";
  std::ofstream file(file_path);
  if (!file) {
    LOG(ERROR) << "open file " << file_path << " failed";
    return false;
  }
  file << res.cpu_cfs_period_us;
  file.close();

  file_path = MountPrefix() + "/" + Name() + "/" + cgroup + "/cpu.cfs_quota_us";
  file.open(file_path);
  if (!file) {
    LOG(ERROR) << "open file " << file_path << " failed" << strerror(errno);
    return false;
  }
  file << res.cpu_cfs_quota_us;
  file.close();

  return true;
}

} // namespace zcontainer