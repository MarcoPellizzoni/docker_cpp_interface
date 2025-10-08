
#include "shell.h"

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


int main(int, char* [])
{
	std::cout << "---------------------- SHELL EXAMPLE START ----------------------\n" << std::endl;
	static std::string input;
	static std::ofstream file_log;
	file_log.open("example_log.txt", std::ios_base::app);
	file_log.clear();

	// test container and notification of inspect_status changes
	while (input != "Q" /*Quit*/)
	{
		std::getline(std::cin, input);
		if (input.empty()) continue;

	}

	return 0;
}
