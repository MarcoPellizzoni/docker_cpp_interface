
#define _WIN32_WINNT 0x0A00 // Target Windows 10 or later (is needed for boost macros)

#include <iostream>
#include <string>
#include "boost/process.hpp"
#include "Shell.h"


struct Shell::ShellImpl
{
	int             ExitStatus = 0;
	std::string     Command;
	std::string     StdIn;
	std::string     StdOut;
	std::string     StdErr;

	ShellImpl() = default;
	~ShellImpl() = default;


	void execute(const std::string& command)
	{
		Command = command;
		this->execute();
	}

	void execute()
	{
		try
		{
			boost::process::ipstream outputStream; // Stream to capture stdout.
			boost::process::ipstream errorStream; // Stream to capture stdout.

			// Run the command and get the standard and error outputs
			boost::process::child process(Command, boost::process::std_out > outputStream, boost::process::std_err > errorStream);

			std::string output;
			std::string line;
			while (outputStream && std::getline(outputStream, line))
			{
				output += line + "\n";
			}

			StdOut = output;

			std::string error_output;
			while (errorStream && std::getline(errorStream, line))
			{
				error_output += line + "\n";
			}

			StdErr = error_output;

			process.wait(); // Wait for the process to finish.

			// exit code of the process after finished
			ExitStatus = process.exit_code();

			return;
		}
		catch (const std::exception& ex)
		{
			ExitStatus = -1;
			StdErr = "Exception: " + std::string(ex.what());
			StdOut = "";
			return;
		}
	}
};

