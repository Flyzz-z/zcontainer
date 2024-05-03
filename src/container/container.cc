
#include "container/container.h"
#include "easylogging++.h"
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <sched.h>
#include <string>
#include <sys/mount.h>
#include <sys/wait.h>
#include <unistd.h>

namespace zcontainer {

int InitAndStartContainer(void *params) {

  LOG(INFO) << "InitContainer";

  Container::RunParams *run_params =
      static_cast<Container::RunParams *>(params);
  if (run_params->cmds.size() == 0) {
    return EXIT_FAILURE;
  }

  // 设置用户映射
  std::ofstream uid_map("/proc/self/uid_map");
  if (uid_map.fail()) {
    LOG(ERROR) << "open uid_map:" << strerror(errno);
    return EXIT_FAILURE;
  }
  uid_map << "0 " << run_params->uid << " 1";
  uid_map.close();

  std::ofstream setgroups("/proc/self/setgroups");
  if (setgroups.fail()) {
    LOG(ERROR) << "open setgroups:" << strerror(errno);
    return EXIT_FAILURE;
  }

  setgroups << "deny";
  setgroups.close();

  // 设置组映射
  std::ofstream gid_map("/proc/self/gid_map");
  if (gid_map.fail()) {
    LOG(ERROR) << "open gid_map:" << strerror(errno);
    return EXIT_FAILURE;
  }
  gid_map << "0 " << run_params->gid << " 1";
  gid_map.close();

  // 挂载操作，先将整个当前的挂载点设置为私有，避免外部影响
  if (mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
    LOG(ERROR) << "mount / private:" << strerror(errno);
    return EXIT_FAILURE;
  }

  // 挂载proc文件系统
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL) ==
      -1) {
    LOG(ERROR) << "mount /proc:" << strerror(errno);
    return EXIT_FAILURE;
  }

  if (!run_params->tty) {
    if (setsid() == -1) {
      LOG(ERROR) << "setsid error:" << strerror(errno);
      return EXIT_FAILURE;
    }

    // 关闭所有继承的文件描述符
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 重定向 stdin, stdout, stderr 到 /dev/null
    open("/dev/null", O_RDWR); // stdin
    dup(0);                    // stdout
    dup(0);                    // stderr
  }

  std::vector<char *> args;
  for (int i = 1; i < run_params->cmds.size(); i++) {
    args.push_back(const_cast<char *>(run_params->cmds[i].c_str()));
  }
  args.push_back(nullptr); // 添加 NULL 结尾

  if (execv(run_params->cmds[0].c_str(), args.data()) == -1) {
    LOG(ERROR) << "exec failed: " << run_params->cmds[0] << strerror(errno);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Container::RunContainer(const RunParams &params) {

  LOG(INFO) << "RunContainer";

  int flags = CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWUSER | CLONE_NEWNS |
              CLONE_NEWNET | CLONE_NEWUTS | SIGCHLD;
  // create stack
  const int stack_size = 1024 * 1024;
  char *stack = (char *)malloc(stack_size);
  int pid =
      clone(InitAndStartContainer, stack + stack_size, flags, (void *)&params);

  LOG(INFO) << "container pid:" << pid;

  if (pid == -1) {
    LOG(ERROR) << "clone error:" << strerror(errno);
    return EXIT_FAILURE;
  }

  // 添加cgourp限制
  Subsystem::ResourceConfig res;
  if (!params.mem.empty()) {
    res.memory_limit_in_bytes = params.mem;
  }
  if (!params.cpu.empty()) {
    res.cpu_cfs_period_us = "100000";
    res.cpu_cfs_quota_us = std::to_string(static_cast<int>(std::stof(params.cpu) * 100000));
  }
  if(!params.cpuset.empty()) {
    res.cpuset_cpus = params.cpuset;
	}


  auto now = std::chrono::steady_clock::now();
  std::string cgroup =
      "zcontainer-" + std::to_string(now.time_since_epoch().count());
  cgroups_manager_.ApplyCgroup(pid, cgroup, res);

  if (waitpid(pid, NULL, 0) == -1) {
    LOG(ERROR) << "waitpid error:" << strerror(errno);
  }
	cgroups_manager_.RemoveCgroup(cgroup);
  return 0;
}
} // namespace zcontainer
