#include "container/container.h"
#include "easylogging++.h"
#include "json.hpp"
#include "network/driver/endpoint.h"
#include "utils.h"
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sched.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

extern char **environ;

namespace zcontainer {

bool CreateNewFs(
    const std::string &root_path,
    const std::vector<std::pair<std::string, std::string>> &volumes) {

  if (mount(NULL, "/", NULL, MS_REC | MS_PRIVATE, NULL) == -1) {
    LOG(ERROR) << "mount / private failed" << strerror(errno);
    return false;
  }

  // 创建目录，并使用overlayfs挂载
  std::string upper_path = root_path + "/upper";
  std::string work_path = root_path + "/work";
  std::string merged_path = root_path + "/merged";

  Utils::createDirectories(upper_path);
  Utils::createDirectories(work_path);
  Utils::createDirectories(merged_path);

  std::string overlay_opts = "lowerdir=" + Container::CONTAINER_STORAGE_PATH +
                             "/overlay2/rootfs" + ",upperdir=" + upper_path +
                             ",workdir=" + work_path;
  if (mount("overlay", merged_path.c_str(), "overlay", 0,
            overlay_opts.c_str()) == -1) {
    LOG(ERROR) << "mount overlay failed" << strerror(errno);
    return false;
  }

  std::string put_old = "/.put_old";
  if (!Utils::createDirectories(merged_path + put_old)) {
    umount2(merged_path.c_str(), MNT_DETACH);
    return false;
  }

  // if (mount(merged_path.c_str(), merged_path.c_str(), NULL, MS_BIND, NULL) ==
  //     -1) {
  //   LOG(ERROR) << "mount bind failed" << strerror(errno);
  //   umount2(merged_path.c_str(), MNT_DETACH);
  //   return false;
  // }

  // 处理volume
  for (const auto &volume : volumes) {
    std::string src = volume.first;
    std::string dest = merged_path + "/" + volume.second;
    Utils::createDirectories(dest);
    if (mount(src.c_str(), dest.c_str(), NULL, MS_BIND, NULL) == -1) {
      LOG(ERROR) << "mount volume failed:" << strerror(errno);
      return false;
    }
  }

  if (syscall(SYS_pivot_root, merged_path.c_str(),
              (merged_path + put_old).c_str()) == -1) {
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

  Utils::removeDirectories(put_old);

  // 挂载proc文件系统
  if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, NULL) ==
      -1) {
    LOG(ERROR) << "mount /proc:" << strerror(errno);
    return false;
  }

  // 挂载sys文件系统
  if (mount("sysfs", "/sys", "sysfs", 0, NULL) == -1) {
    LOG(ERROR) << "mount /sys:" << strerror(errno);
    return false;
  }

  // 挂载/dev
  if (mount("tmpfs", "/dev", "tmpfs", 0, NULL) == -1) {
    LOG(ERROR) << "mount /dev:" << strerror(errno);
    return false;
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

  // 处理volume参数
  std::vector<std::pair<std::string, std::string>> volumes;
  for (const auto &volume : run_params->volumes) {
    // : split
    auto pos = volume.find(":");
    if (pos == std::string::npos) {
      LOG(ERROR) << "volume format error";
      return EXIT_FAILURE;
    }

    std::string src = volume.substr(0, pos);
    std::string dest = volume.substr(pos + 1);
    volumes.push_back(std::make_pair(src, dest));
  }

  // 创建新的文件系统
  std::string fs_path = Container::CONTAINER_STORAGE_PATH + "/overlay2/" +
                        run_params->container_id;
  if (!Utils::createDirectories(fs_path)) {
    return EXIT_FAILURE;
  }
  CreateNewFs(fs_path, volumes);

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
  for (int i = 0; i < run_params->cmds.size(); i++) {
    args.push_back(const_cast<char *>(run_params->cmds[i].c_str()));
  }
  args.push_back(nullptr); // 添加 NULL 结尾

  // 处理环境变量
  std::vector<char *> envs;
  char **p_env = environ;
  for (; *p_env != nullptr; p_env++) {
    envs.push_back(*p_env);
  }
  for (const auto &env : run_params->envs) {
    envs.push_back(const_cast<char *>(env.c_str()));
  }
  envs.push_back(nullptr);

  if (execve(run_params->cmds[0].c_str(), args.data(), envs.data()) == -1) {
    LOG(ERROR) << "exec failed: " << run_params->cmds[0] << strerror(errno);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Container::RunContainer(RunParams &params) {

  LOG(INFO) << "RunContainer";
  auto now = std::chrono::steady_clock::now();
  container_id_ = std::to_string(now.time_since_epoch().count());
  params.container_id = container_id_;

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

  // 创建容器目录，添加容器配置
  std::string container_path =
      CONTAINER_STORAGE_PATH + "/container/" + container_id_;
  if (!Utils::createDirectories(container_path)) {
    return EXIT_FAILURE;
  }
  nlohmann::json container_json;
  container_json["container_id"] = container_id_;
  container_json["pid"] = pid;
  container_json["cmd"] = params.cmds;
  container_json["cpu limit"] = params.cpu;
  container_json["mem limit"] = params.mem;
  container_json["cpuset limit"] = params.cpuset;
  container_json["volumes"] = params.volumes;
  std::ofstream ofs(container_path + "/config.json");
  ofs << container_json.dump(4);
  ofs.close();

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

  std::string cgroup = "zc-" + container_id_ + "-" + std::to_string(pid);
  cgroups_manager_.ApplyCgroup(pid, cgroup, res);

  Endpoint endpoint;
  if (!params.network.empty()) {
    std::string ln_src = "/proc/" + std::to_string(pid) + "/ns/net";
    std::string ln_dest = "/var/run/netns/" + container_id_;
    system(("ln -s " + ln_src + " " + ln_dest).c_str());
    endpoint.id_ = params.container_id;
    endpoint.container_pid_ = pid;
    endpoint.port_mappings_ = params.port_mappings;
    network_manager_.ConnectToNetwork(endpoint, params.network);
  }

  if (!params.daemon) {
    if (waitpid(pid, NULL, 0) == -1) {
      LOG(ERROR) << "waitpid error:" << strerror(errno);
    }
    cgroups_manager_.RemoveCgroup(cgroup);
    if (!params.network.empty()) {
      network_manager_.DisconnectFromNetwork(endpoint);
      remove(("/var/run/netns/" + container_id_).c_str());
    }
  }
  return 0;
}

bool Container::ExecContainer(ExecParams &params) {
  LOG(INFO) << "ExecContainer";
  if (params.cmds.size() == 0) {
    return false;
  }
  std::string container_path =
      CONTAINER_STORAGE_PATH + "/container/" + params.container_id;
  std::ifstream ifs(container_path + "/config.json");
  if (!ifs.is_open()) {
    LOG(ERROR) << "container config file not found";
    return false;
  }

  nlohmann::json container_json;
  ifs >> container_json;
  ifs.close();

  int container_pid = container_json["pid"];

  // 处理环境变量
  std::vector<char *> envs;
  char **p_env = environ;
  for (; *p_env != NULL; p_env++) {
    LOG(INFO) << *p_env;
    envs.push_back(*p_env);
  }

  ifs.open("/proc/" + std::to_string(container_pid) + "/environ");
  if (!ifs.is_open()) {
    LOG(ERROR) << "open container environ file failed" << strerror(errno);
    return false;
  }
  std::string container_env;
  while (std::getline(ifs, container_env, '\0')) {
    envs.push_back(const_cast<char *>(container_env.c_str()));
  }
  envs.push_back(nullptr);

  // 加入ns
  std::vector<std::string> ns_list = {"ipc", "pid", "uts", "net", "mnt"};
  for (const auto &ns : ns_list) {
    std::string ns_path =
        "/proc/" + std::to_string(container_pid) + "/ns/" + ns;
    int fd = open(ns_path.c_str(), O_RDONLY);
    if (setns(fd, 0) == -1) {
      LOG(ERROR) << "setns error:" << strerror(errno);
      return false;
    }
    close(fd);
  }

  std::vector<char *> args;
  for (int i = 0; i < params.cmds.size(); i++) {
    args.push_back(const_cast<char *>(params.cmds[i].c_str()));
  }
  args.push_back(nullptr); // 添加 NULL 结尾
  if (execve(args[0], args.data(), envs.data()) == -1) {
    LOG(ERROR) << "exec failed:" << strerror(errno);
    return false;
  }
  return true;
}
}; // namespace zcontainer
