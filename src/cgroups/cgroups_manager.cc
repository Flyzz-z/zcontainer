#include "cgroups/cgroups_manager.h"
#include "cgroups/subsystem/cpu_subsystem.h"
#include "cgroups/subsystem/cpuset_subsystem.h"
#include "cgroups/subsystem/memory_subsystem.h"
#include "cgroups/subsystem/subsystem.h"
#include "easylogging++.h"
#include <memory>

namespace zcontainer {

CgroupsManager::CgroupsManager() {
  subsystems_.push_back(std::make_unique<CpuSubsystem>());
  subsystems_.push_back(std::make_unique<MemorySubsystem>());
  subsystems_.push_back(std::make_unique<CpusetSubsystem>());
}

bool CgroupsManager::ApplyCgroup(int pid, const std::string &cgroup,
                                 const Subsystem::ResourceConfig &res) {
  for (auto &subsystem : subsystems_) {
    LOG(INFO) << "try Applying cgroup " << subsystem->Name() << " " << cgroup;
    if (!subsystem->Create(cgroup)) {
      LOG(ERROR) << "create cgroup failed" << subsystem->Name() << " "
                 << cgroup;
      return false;
    }

    if (!subsystem->SetResourceLimit(cgroup, res)) {
      subsystem->Remove(cgroup);
      continue;
    }
    if (!subsystem->Apply(pid, cgroup)) {
      LOG(ERROR) << "apply cgroup failed" << subsystem->Name() << " " << cgroup;
      return false;
    }
  }
  return true;
}

void CgroupsManager::RemoveCgroup(const std::string &cgroup) {
  for (auto &subsystem : subsystems_) {
    subsystem->Remove(cgroup);
  }
}
} // namespace zcontainer