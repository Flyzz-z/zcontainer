#include "CLI11.hpp"
#include "container/container.h"
#include "easylogging++.h"
#include <iostream>
#include <unistd.h>
#include <vector>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {
  CLI::App app{"zcontainer - a toy container runtime written in C++"};
  app.require_subcommand(0, 1);

  CLI::App *run_cmd =
      app.add_subcommand("run", "run a command in a new container");
  bool run_flag_tty{false};
  std::string run_option_mem;
  std::string run_option_cpu;
  std::string run_option_cpuset;
  std::vector<std::string> cmds;
  run_cmd->add_flag("--it", run_flag_tty, "enable tty");
  run_cmd->add_option("--mem", run_option_mem, "memory limit");
  run_cmd->add_option("--cpu", run_option_cpu, "cpu limit");
  run_cmd->add_option("--cpuset", run_option_cpuset, "cpuset limit");
  run_cmd->add_option("cmd", cmds, "command to run in the container");
  // TODO: 参数校验

  // parse the command line arguments
  CLI11_PARSE(app, argc, argv);

  // handle the subcommands
  if (*run_cmd) {
    zcontainer::Container container;
    zcontainer::Container::RunParams run_params;
    run_params.tty = run_flag_tty;
    run_params.cmds = cmds;
    run_params.mem = run_option_mem;
    run_params.cpu = run_option_cpu;
    run_params.cpuset = run_option_cpuset;
    run_params.uid = getuid();
    run_params.gid = getgid();
    container.RunContainer(run_params);
  }
  return 0;
}