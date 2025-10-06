
#include "Docker.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include "filesystem"
#include <sstream>
#include <vector>


void print(std::string log)
{
	std::cout << log << std::endl;
}

void status_notification(docker::Container& container, docker::Container::Status inspect_status)
{
	switch (inspect_status) {
	case docker::Container::CREATED:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> CREATED");
		break;
	case docker::Container::RESTARTING:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> RESTARTING");
		break;
	case docker::Container::RUNNING:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> RUNNING");
		break;
	case docker::Container::REMOVING:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> REMOVING");
		break;
	case docker::Container::PAUSED:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> PAUSED");
		break;
	case docker::Container::EXITED:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> EXITED");
		break;
	case docker::Container::DEAD:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> DEAD");
		break;
	case docker::Container::UNKNOWN:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> INVALID");
		break;
	case docker::Container::REMOVED:
		print(container.get_runtime_infos().name + "::::::: status changed!! --> REMOVED");
		break;
	}
}


int main(int, char* [])
{
	std::cout << "---------------------- EXAMPLE START ----------------------\n" << std::endl;

	using namespace docker;

	// select an image
    std::string requested_image = "test_maas1:latest";

	// prepare a exec_run blueprint
	auto runTestMAAS = CLI::Run(requested_image)
		.add_tty()
		.detached()
		.set_env("DISPLAY","$DISPLAY")
		.set_env("QT_X11_NO_MITSHM", "1")
		.workdir("/home/install/bin");

	static std::string input;
	static std::ofstream file_log;

	file_log.open("example_log.txt", std::ios_base::app);
	file_log.clear();

	// create a container with the exec_run blueprint
	Container test_container(runTestMAAS);

	// add a callback for the inspect_status changed notifications
	test_container.set_status_callback(status_notification);

	std::vector<Container> containers;

	// test container and notification of inspect_status changes
	while (input != "Q" /*Quit*/)
	{
		std::string result;

		std::getline(std::cin, input);
		if (input.empty()) continue;

		std::vector<std::string> strings;
        utils::split_string(input, ' ', [&strings](std::string s) {strings.emplace_back(s); });

		if (strings.front() == "run")
		{
			// set the name
			auto name = strings.back();
			test_container.set_container_name(name);
			// run and get std output result
			result = test_container.exec_run().second;

			containers.emplace_back(test_container);
		}

		if (strings.front() == "inspect")
		{
			auto name = strings.back();
			auto ret = CLI::Inspect(name).extract(CLI::Inspect::Extract::IMAGE_ID).execute();

			print(ret.second);
		}

		if (strings.front() == "start")
		{
			// start the stopped container
			auto name = strings.back();
			result = test_container.exec_start().second;
		}

		if (strings.front() == "stop")
		{
			// stop the running container
			auto name = strings.back();
			result = test_container.exec_stop().second;
		}

		if (strings.front() == "rm")
		{
         // remove the container
			auto name = strings.back();
			result = test_container.exec_remove().second;
		}

		if (strings.front() == "prune")
		{
			// removes all stopped containers
			result = CLI::Prune().execute().second;
		}

		std::cout << result << std::endl;

		file_log << result << std::endl;
	}

	return 0;
}
