#ifndef CGROUPS_CGROUPS_MANAGER_H
#define CGROUPS_CGROUPS_MANAGER_H

#include "cgroups/subsystem/subsystem.h"
#include <memory>
#include <string>
#include <vector>

namespace zcontainer {
class CgroupsManager {
public:
  CgroupsManager();
  ~CgroupsManager() = default;
  CgroupsManager(const CgroupsManager &) = delete;
  CgroupsManager &operator=(const CgroupsManager &) = delete;
  CgroupsManager(CgroupsManager &&) = delete;
  CgroupsManager &operator=(CgroupsManager &&) = delete;

  /**
   * @brief
   * 当前模型为每次执行创建进程，为该进程创建cgroup,即一个cgroup对应一个进程，进程结束即删除cgroup
   *
   * @param pid process id
   * @param cgroup cgroup name
   * @param res resource config
   * @return true apply cgroup successfully
   * @return false apply cgroup failed
   */
  bool ApplyCgroup(int pid, const std::string &cgroup,
                   const Subsystem::ResourceConfig &res);
  void RemoveCgroup(const std::string &cgroup);

private:
  std::vector<std::unique_ptr<Subsystem>> subsystems_;
};
} // namespace zcontainer

#endif