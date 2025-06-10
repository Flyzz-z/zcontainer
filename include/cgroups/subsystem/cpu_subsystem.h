#ifndef CGROUPS_SUBSYSTEM_CPU_SUBSYSTEM_H
#define CGROUPS_SUBSYSTEM_CPU_SUBSYSTEM_H

#include "subsystem.h"

namespace zcontainer {

class CpuSubsystem : public Subsystem {

public:
  CpuSubsystem();
  ~CpuSubsystem() = default;
  CpuSubsystem(const CpuSubsystem &) = delete;
  CpuSubsystem &operator=(const CpuSubsystem &) = delete;
  CpuSubsystem(CpuSubsystem &&) = delete;
  CpuSubsystem &operator=(CpuSubsystem &&) = delete;

  bool SetResourceLimit(const std::string &cgroup,
                        const ResourceConfig &res) override;
};
} // namespace zcontainer

#endif