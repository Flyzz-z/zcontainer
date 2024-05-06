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
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

namespace zcontainer {

bool CreateNewFs(const std::string &container_id) {

  if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
    LOG(ERROR) << "mount / private failed" << strerror(errno);
    return false;
  }

  // 创建目录，并使用overlayfs挂载
  std::string container_path =
      Container::CONTAINER_STORAGE_PATH + "/" + container_id;
  std::string upper_path = container_path + "/upper";
  std::string work_path = container_path + "/work";
  std::string merged_path = container_path + "/merged";
  mkdir(container_path.c_str(), 0755);
  mkdir(upper_path.c_str(), 0755);
  mkdir(work_path.c_str(), 0755);
  mkdir(merged_path.c_str(), 0755);

  std::string overlay_opts = "lowerdir=" + Container::CONTAINER_STORAGE_PATH +
                             "/rootfs" + ",upperdir=" + upper_path +
                             ",workdir=" + work_path;
  if (mount("overlay", merged_path.c_str(), "overlay", 0,
            overlay_opts.c_str()) == -1) {
    LOG(ERROR) << "mount overlay failed" << strerror(errno);
    return false;
  }

  std::string put_old = "/.put_old";
  if (mkdir((merged_path + put_old).c_str(), 0755) == -1) {
    LOG(ERROR) << "mkdir /.put_old failed" << strerror(errno);
    umount2(merged_path.c_str(), MNT_DETACH);
    return false;
  }

  if (mount(merged_path.c_str(), merged_path.c_str(), NULL, MS_BIND, NULL) ==
      -1) {
    LOG(ERROR) << "mount bind failed" << strerror(errno);
    umount2(merged_path.c_str(), MNT_DETACH);
    return false;
  }

  if (syscall(SYS_pivot_root, merged_path.c_str(), (merged_path + put_old).c_str()) == -1) {
    LOG(ERROR) << "pivot_root failed" << strerror(errno);
    umount2(merged_path.c_str(), MNT_DETACH);
    return false;
  }

  if (chdir("/") == -1) {
    LOG(ERROR) << "chdir / failed" << strerror(errno);
    return false;
  }

  if (umount2(put_old.c_str(), MNT_DETACH) == -1) {
    LOG(ERROR) << "umount /.put_old failed" << strerror(errno);
    return false;
  }

  if (rmdir(put_old.c_str()) == -1) {
    LOG(ERROR) << "rmdir /.put_old failed" << strerror(errno);
    return false;
  }

  // 挂载proc文件系统
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL) ==
      -1) {
    LOG(ERROR) << "mount /proc:" << strerror(errno);
    return EXIT_FAILURE;
  }

  // 挂载sys文件系统
  if (mount("sysfs", "/sys", "sysfs", 0, NULL) == -1) {
    LOG(ERROR) << "mount /sys:" << strerror(errno);
    return EXIT_FAILURE;
  }

  // 挂载/dev
  if (mount("tmpfs", "/dev", "tmpfs", 0, NULL) == -1) {
    LOG(ERROR) << "mount /dev:" << strerror(errno);
    return EXIT_FAILURE;
  }

  return true;
}

const std::string Container::CONTAINER_STORAGE_PATH = "/var/lib/zcontainer";

int InitAndStartContainer(void *params) {

  LOG(INFO) << "InitContainer";

  Container::RunParams *run_params =
      static_cast<Container::RunParams *>(params);
  if (run_params->cmds.size() == 0) {
    return EXIT_FAILURE;
  }

  // 不做用户态了，直接使用root权限，弄不明白
  // // 设置用户映射
  // std::ofstream uid_map("/proc/self/uid_map");
  // if (uid_map.fail()) {
  //   LOG(ERROR) << "open uid_map:" << strerror(errno);
  //   return EXIT_FAILURE;
  // }
  // uid_map << "0 " << run_params->uid << " 1";
  // uid_map.close();

  // std::ofstream setgroups("/proc/self/setgroups");
  // if (setgroups.fail()) {
  //   LOG(ERROR) << "open setgroups:" << strerror(errno);
  //   return EXIT_FAILURE;
  // }

  // setgroups << "deny";
  // setgroups.close();

  // // 设置组映射
  // std::ofstream gid_map("/proc/self/gid_map");
  // if (gid_map.fail()) {
  //   LOG(ERROR) << "open gid_map:" << strerror(errno);
  //   return EXIT_FAILURE;
  // }
  // gid_map << "0 " << run_params->gid << " 1";
  // gid_map.close();

  // 在创建新rootfs中创建
  // // 挂载操作，先将整个当前的挂载点设置为私有，避免外部影响
  // if (mount("none", "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
  //   LOG(ERROR) << "mount / private:" << strerror(errno);
  //   return EXIT_FAILURE;
  // }

  // // 挂载proc文件系统
  // if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL)
  // ==
  //     -1) {
  //   LOG(ERROR) << "mount /proc:" << strerror(errno);
  //   return EXIT_FAILURE;
  // }

  // 创建新的文件系统
  CreateNewFs(run_params->container_id);

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

int Container::RunContainer(RunParams &params) {

  LOG(INFO) << "RunContainer";
  if (container_id_.empty()) {
    auto now = std::chrono::steady_clock::now();
    container_id_ = std::to_string(now.time_since_epoch().count());
    params.container_id = container_id_;
  }

  // 创建容器目录
  std::string container_path = CONTAINER_STORAGE_PATH + "/" + container_id_;
  if (mkdir(container_path.c_str(), 0755) == -1) {
    LOG(ERROR) << "create container dir " << container_path << " failed"
               << strerror(errno);
    return EXIT_FAILURE;
  }

  int flags = CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET |
              CLONE_NEWUTS | SIGCHLD;
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
    res.cpu_cfs_quota_us =
        std::to_string(static_cast<int>(std::stof(params.cpu) * 100000));
  }
  if (!params.cpuset.empty()) {
    res.cpuset_cpus = params.cpuset;
  }

  std::string cgroup =
      "zcontainer-" + container_id_ + "-" + std::to_string(pid);
  cgroups_manager_.ApplyCgroup(pid, cgroup, res);

  if (waitpid(pid, NULL, 0) == -1) {
    LOG(ERROR) << "waitpid error:" << strerror(errno);
  }
  cgroups_manager_.RemoveCgroup(cgroup);
  return 0;
}
}; // namespace zcontainer
