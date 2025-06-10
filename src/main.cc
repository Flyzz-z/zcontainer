#include "CLI11.hpp"
#include "container/container.h"
#include "easylogging++.h"
#include "network/network_manager.h"
#include "utils.h"
#include <iostream>
#include <unistd.h>
#include <vector>

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {

  // 初始化
  zcontainer::Utils::createDirectories(
      zcontainer::Container::CONTAINER_STORAGE_PATH + "/container");
  zcontainer::Utils::createDirectories(
      zcontainer::Container::CONTAINER_STORAGE_PATH + "/overlay2");
  zcontainer::Utils::createDirectories(
      zcontainer::Container::CONTAINER_STORAGE_PATH + "/network");
  zcontainer::Utils::createDirectories(
      zcontainer::Container::CONTAINER_STORAGE_PATH + "/network/networks");

  CLI::App app{"zcontainer - a toy container runtime written in C++"};
  app.require_subcommand(0, 1);

  // zcontainer run --it --mem 100M --cpu 0.5 --cpuset 0-1 ls -l
  CLI::App *run_cmd =
      app.add_subcommand("run", "run a command in a new container");
  bool run_flag_tty{false};
  bool run_flag_daemon{false};
  std::string run_option_mem;
  std::string run_option_cpu;
  std::string run_option_cpuset;
  std::string run_option_net;
  std::vector<std::string> run_option_volumes;
  std::vector<std::string> run_option_envs;
  std::vector<std::string> run_option_cmds;
  std::vector<std::string> run_option_ports;
  run_cmd->add_flag("--it", run_flag_tty, "enable tty");
  run_cmd->add_flag("-d", run_flag_daemon, "run in background");
  run_cmd->add_option("--mem", run_option_mem, "memory limit");
  run_cmd->add_option("--cpu", run_option_cpu, "cpu limit");
  run_cmd->add_option("--cpuset", run_option_cpuset, "cpuset limit");
  run_cmd->add_option("-v,--volumes", run_option_volumes, "volumes")
      ->allow_extra_args(false);
  run_cmd->add_option("-e,--env", run_option_envs, "env")
      ->allow_extra_args(false);
  run_cmd->add_option("-p,--port", run_option_ports, "port")
      ->allow_extra_args(false);
  run_cmd->add_option("--net", run_option_net, "network");
  run_cmd->add_option("cmd", run_option_cmds,
                      "command to run in the container");
  // TODO: 参数校验

  // exec
  CLI::App *exec_cmd =
      app.add_subcommand("exec", "exec a command in a running container");
  bool exec_flag_tty{false};
  std::string exec_option_container_id;
  std::vector<std::string> exec_option_cmds;
  exec_cmd->add_flag("--it", exec_flag_tty, "enable tty");
  exec_cmd->add_option("container_id", exec_option_container_id,
                       "container id");
  exec_cmd->add_option("cmd", exec_option_cmds,
                       "command to run in the container");

  CLI::App *network_cmd = app.add_subcommand("network", "network management");
  CLI::App *network_create_cmd =
      network_cmd->add_subcommand("create", "create a network");
  std::string network_create_option_name;
  std::string network_create_option_driver;
  std::string network_create_option_subnet;
  network_create_cmd->add_option("--name", network_create_option_name,
                                 "network name");
  network_create_cmd->add_option("--driver", network_create_option_driver,
                                 "network driver");
  network_create_cmd->add_option("--subnet", network_create_option_subnet,
                                 "network subnet");

  CLI::App *network_list_cmd =
      network_cmd->add_subcommand("list", "list all network");
  CLI::App *network_delete_cmd =
      network_cmd->add_subcommand("delete", "delete a network");
  std::string network_delete_option_name;
  network_delete_cmd->add_option("--name,name", network_delete_option_name,
                                 "network name");

  // parse the command line arguments
  CLI11_PARSE(app, argc, argv);

  // handle the subcommands
  if (*run_cmd) {
    zcontainer::Container container;
    zcontainer::Container::RunParams run_params;
    run_params.tty = run_flag_tty;
    run_params.daemon = run_flag_daemon;
    run_params.mem = run_option_mem;
    run_params.cpu = run_option_cpu;
    run_params.cpuset = run_option_cpuset;
    run_params.network = run_option_net;
    run_params.volumes = run_option_volumes;
    run_params.envs = run_option_envs;
    run_params.port_mappings = run_option_ports;
    run_params.cmds = run_option_cmds;
    container.RunContainer(run_params);
  } else if (*exec_cmd) {
    zcontainer::Container container;
    zcontainer::Container::ExecParams exec_params;
    exec_params.tty = exec_flag_tty;
    exec_params.container_id = exec_option_container_id;
    exec_params.cmds = exec_option_cmds;
    container.ExecContainer(exec_params);
  } else if (*network_cmd) {
    zcontainer::NetworkManager network_manager;
    if (*network_create_cmd) {
      network_manager.CreateNetwork(network_create_option_name,
                                    network_create_option_driver,
                                    network_create_option_subnet);
    } else if (*network_list_cmd) {
      auto networks = network_manager.ListNetworks();
      for (const auto &network : networks) {
        network.Print();
      }
    } else if (*network_delete_cmd) {
      network_manager.DeleteNetwork(network_delete_option_name);
    }
  }
  return 0;
}