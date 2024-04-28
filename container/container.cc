#include "container.h"
#include <cstddef>
#include <cstdlib>
#include <fcntl.h>
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
  FILE *uid_map = fopen("/proc/self/uid_map", "w");
  if (uid_map == NULL) {
    perror("open uid_map");
    return EXIT_FAILURE;
  }
  fprintf(uid_map, "0 %d 1",
          run_params->uid); // 映射宿主系统的 root 用户到新的 user namespace
  fclose(uid_map);

  FILE *setgroups_file = fopen("/proc/self/setgroups", "w");
  if (setgroups_file == NULL) {
    perror("open setgroups");
    return EXIT_FAILURE;
  }

  // 写入"deny"到/setgroups文件
  fprintf(setgroups_file, "deny");
  // 关闭/setgroups文件
  fclose(setgroups_file);

  // 设置组映射
  FILE *gid_map = fopen("/proc/self/gid_map", "w");
  if (gid_map == NULL) {
    perror("open gid_map");
    return EXIT_FAILURE;
  }
  fprintf(gid_map, "0 %d 1",
          run_params->gid); // 映射宿主系统的 root 组到新的 user namespace
  fclose(gid_map);

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

  if (waitpid(pid, NULL, 0) == -1) {
    LOG(ERROR) << "waitpid error:" << strerror(errno);
  }

  return 0;
}
} // namespace zcontainer
