#ifndef CGROUPS_SUBSYSTEM_MEMORY_SUBSYSTEM_H
#define CGROUPS_SUBSYSTEM_MEMORY_SUBSYSTEM_H

#include "cgroups/subsystem/subsystem.h"

namespace zcontainer {

class MemorySubsystem : public Subsystem {
public:
  MemorySubsystem();
  ~MemorySubsystem() = default;
  MemorySubsystem(const MemorySubsystem &) = delete;
  MemorySubsystem &operator=(const MemorySubsystem &) = delete;
  MemorySubsystem(MemorySubsystem &&) = delete;
  MemorySubsystem &operator=(MemorySubsystem &&) = delete;

  bool SetResourceLimit(const std::string &cgroup,
                        const ResourceConfig &res) override;
};
}; // namespace zcontainer

#endif
