#include "cgroups/subsystem/subsystem.h"

namespace zcontainer {

class CpusetSubsystem : public Subsystem {
public:
  CpusetSubsystem();
  ~CpusetSubsystem() = default;
  CpusetSubsystem(const CpusetSubsystem &) = delete;
  CpusetSubsystem &operator=(const CpusetSubsystem &) = delete;
  CpusetSubsystem(CpusetSubsystem &&) = delete;
  CpusetSubsystem &operator=(CpusetSubsystem &&) = delete;

  bool SetResourceLimit(const std::string &cgroup,
                        const ResourceConfig &res) override;
};
}; // namespace zcontainer