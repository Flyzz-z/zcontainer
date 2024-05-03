#ifndef CGROUPS_SUBSYSTEM_SUBSYSTEM_H
#define CGROUPS_SUBSYSTEM_SUBSYSTEM_H

#include <string>

namespace zcontainer {
enum class SubsystemType {
  kCpu,
  kCpuset,
  kMemory,
};

class Subsystem {

public:
  Subsystem() = delete;
  virtual ~Subsystem() = default;
  Subsystem(const std::string &name, SubsystemType subsystem_type)
      : name_(name), subsystem_type_(subsystem_type) {}
  Subsystem(const Subsystem &) = delete;
  Subsystem &operator=(const Subsystem &) = delete;
  Subsystem(Subsystem &&) = delete;
  Subsystem &operator=(Subsystem &&) = delete;

  struct ResourceConfig {
    std::string memory_limit_in_bytes{};
    std::string cpu_cfs_period_us{};
    std::string cpu_cfs_quota_us{};
    std::string cpuset_cpus{};
  };

  std::string Name() { return name_; }
  std::string MountPrefix() { return mount_prefix_; }
  SubsystemType SubsystemType() { return subsystem_type_; }
  bool Create(const std::string &cgroup);
  virtual bool SetResourceLimit(const std::string &cgroup,
                                const ResourceConfig &res) = 0;
  bool Apply(int pid, const std::string &cgroup);
  void Remove(const std::string &cgroup);

private:
  std::string name_;
  std::string mount_prefix_{"/sys/fs/cgroup"};
  enum SubsystemType subsystem_type_;
};

} // namespace zcontainer

#endif