
#include "docker.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include "filesystem"
#include <sstream>
#include <vector>



void status_notification(docker::Container* container)
{
	auto infos = container->get_runtime_infos();
	std::cout << "Container " << infos.name << " changed its status to " << infos.current_status << std::endl;
}


int main(int, char* [])
{
	std::cout << "---------------------- CONTAINER EXAMPLE START ----------------------\n" << std::endl;

	using namespace docker;
	static std::string input;
	static std::ofstream file_log;

	file_log.open("example_log.txt", std::ios_base::app);
	file_log.clear();

	// choose an image to run (mandatory)
   std::string image_name = "test_image:latest";
	std::cout << docker::CLI::Images().execute().second << std::endl;
	std::cout << "Choose Image: " << std::endl;
	std::getline(std::cin, image_name);

	// choose a name for the container (optional)
	std::string container_name = "test_container";
	std::cout << "Choose a name or leave empty: " << std::endl;
	std::getline(std::cin, container_name);

	// Construct the command with the options for creating the container
	auto container_creator = CLI::Create(image_name)
		.add_tty()
		.set_env("DISPLAY","$DISPLAY")
		.workdir("/home");

	// Create the container object
	Container test_container{ container_creator, container_name };
	test_container.exec_create();

	// add a callback for the inspect_status changed notifications
	test_container.set_status_callback(status_notification);

	std::vector<Container> containers;

	// test container and notification of inspect_status changes
	while (input != "Q" && input != "q" /*Quit*/)
	{
		Shell::Output result;

		std::getline(std::cin, input);
		if (input.empty()) continue;

		std::vector<std::string> strings;
      utils::split_string(input, ' ', [&strings](std::string s) {strings.emplace_back(s); });

		if (strings.front() == "inspect")
		{
			std::cout << CLI::Inspect(test_container.get_runtime_infos().name).extract(CLI::Inspect::Extract::ID).execute().second << "\n"
				<< CLI::Inspect(test_container.get_runtime_infos().name).extract(CLI::Inspect::Extract::IMAGE_ID).execute().second << "\n"
				<< CLI::Inspect(test_container.get_runtime_infos().name).extract(CLI::Inspect::Extract::STATUS).execute().second << std::endl;
		}

		if (strings.front() == "start")
		{
			// start the stopped container
			auto name = strings.back();
			result = test_container.exec_start();
		}

		if (strings.front() == "stop")
		{
			// stop the running container
			auto name = strings.back();
			result = test_container.exec_stop();
		}

		if (strings.front() == "rm")
		{
         // remove the container
			auto name = strings.back();
			result = test_container.exec_remove();
		}

		if (strings.front() == "prune")
		{
			// removes all stopped containers
			result = CLI::Prune().execute();
		}

		file_log 
			<< "\nCommand: " << strings.front() 
			<< "\nExit code: " << result.first 
			<< "\nResult: " << result.second 
			<< std::endl;

	}

	return 0;
}
