#include "CLI11.hpp"
#include <iostream>
#include <unistd.h>
#include "container/container.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char **argv) {
		CLI::App app{"zcontainer - a toy container runtime written in C++"};
		app.require_subcommand(0, 1);

		CLI::App *init_cmd = app.add_subcommand("init","initialize the container");
		

		CLI::App *run_cmd = app.add_subcommand("run","run a command in a new container");
		bool run_flag_tty{false};
		std::vector<std::string> cmds;
		run_cmd->add_flag("-t",run_flag_tty,"enable tty");
		run_cmd->add_option("cmd",cmds,"command to run in the container");
		//TODO: 参数校验


		// parse the command line arguments
		CLI11_PARSE(app, argc, argv);

		// handle the subcommands
		if(*init_cmd) {
			
		} else if(*run_cmd) {
			zcontainer::Container container;
			zcontainer::Container::RunParams run_params;
			run_params.tty = run_flag_tty;
			run_params.cmds = cmds;
			run_params.uid = getuid();
			run_params.gid = getgid();
			container.RunContainer(run_params);
		}
		return 0;
}