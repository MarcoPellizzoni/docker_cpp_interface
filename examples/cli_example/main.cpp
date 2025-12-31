
#include "Docker.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include "filesystem"
#include <sstream>
#include <vector>


int main(int, char* [])
{
	std::cout << "---------------------- CLI EXAMPLE START ----------------------\n" << std::endl;

	static std::ofstream file_log;
	file_log.open("example_log.txt", std::ios_base::app);
	file_log.clear();

	static std::string input;

	while (input != "Q" /*Quit*/)
	{
		std::string result;

		std::getline(std::cin, input);
		if (input.empty()) continue;

		std::vector<std::string> strings;
        docker::utils::split_string(input, ' ', [&strings](std::string s) {strings.emplace_back(s); });

		if (strings.front() == "inspect")
		{
			auto name = strings.back();
			auto ret = docker::CLI::Inspect(name).extract(docker::CLI::Inspect::Extract::IMAGE_ID).execute();
		}

		std::cout << result << std::endl;

		file_log << result << std::endl;
	}

	return 0;
}
