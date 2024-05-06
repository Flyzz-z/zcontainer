#include "cgroups/subsystem/memory_subsystem.h"
#include "cgroups/subsystem/subsystem.h"
#include "easylogging++.h"
#include <cstring>
#include <unistd.h>

namespace zcontainer {

MemorySubsystem::MemorySubsystem()
    : Subsystem("memory", SubsystemType::kMemory) {}

bool MemorySubsystem::SetResourceLimit(const std::string &cgroup,
                                       const ResourceConfig &res) {
  if (res.memory_limit_in_bytes.empty()) {
    return false;
  }
  std::string file_path =
      MountPrefix() + "/" + Name() + "/" + cgroup + "/memory.limit_in_bytes";
  std::ofstream file(file_path);
  if (!file) {
    LOG(ERROR) << "open file " << file_path << " failed";
    return false;
  }
  file << res.memory_limit_in_bytes;
  file.close();
  return true;
}
}; // namespace zcontainer