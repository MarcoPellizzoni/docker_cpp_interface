#include "Shell.h"

/*
* Select what implementation to use
*/
#ifdef USE_BOOST
#include "ShellBoost.hpp" // cross platform
#endif // USE_BOOST
#ifdef USE_WIN32
#include "ShellWin32.hpp"
#endif // USE_WIN32
#ifdef USE_UNIX
#include "ShellUnix.hpp"
#endif // USE_UNIX

#include <iostream>
/*
* Define methods using bridge
*/
std::pair<int, std::string> Shell::execute()
{
	_pimpl->execute();
	
	auto exit_code = _pimpl->ExitStatus;
	auto error_out = _pimpl->StdErr;
	auto std_out = _pimpl->StdOut;
	
	// clean up output from end final lines
	if(!error_out.empty() && error_out.back() == '\n') error_out.erase(error_out.end() -1);
	if(!std_out.empty() && std_out.back() == '\n') std_out.erase(std_out.end() -1);

	// catch the result 
	if (std_out.empty())
	{
		_result = error_out;
	}
	else
	{
		_result = std_out;
	}

	// catch the exit code
	_exit_status = exit_code;
	
	// clean outputs of pimpl
	_pimpl->StdErr="";
	_pimpl->StdOut="";
	_pimpl->ExitStatus = 0;

	// return 
	std::pair<int, std::string> result;
	result.first = _exit_status;
	result.second = _result;
	return result;
}

std::pair<int, std::string> Shell::execute(const Shell::Input command)
{
	this->setCommand(command);
	return this->execute();
}

Shell::Output2 Shell::prompt(const Input command)
{
	Shell::ShellImpl shell;

	shell.execute(command);

	auto exit_code = shell.ExitStatus;
	auto error_out = shell.StdErr;
	auto std_out = shell.StdOut;

	// clean up output from end final lines
	if (!error_out.empty() && error_out.back() == '\n') error_out.erase(error_out.end() - 1);
	if (!std_out.empty() && std_out.back() == '\n') std_out.erase(std_out.end() - 1);

	// catch the result 
	std::string result;
	std_out.empty() ? result = error_out : result = std_out;

	Shell::Output2 o;
	o.exitCode = (Exit)exit_code;
	o.result = std_out;

	return o;
}

void Shell::setCommand(const Shell::Input cmd) noexcept
{
	_command = cmd;
	_pimpl->Command = cmd;
}

Shell::Input Shell::getCommand() const noexcept
{
	return _command;
}

int Shell::getExitStatus() const noexcept
{
	return _exit_status;
}

std::string Shell::getResult() const noexcept
{
	return _result;
}



/*
* Define special member functions
*/
Shell::Shell()
	: _pimpl(std::make_unique<ShellImpl>())
{
}

Shell::Shell(Shell::Input cmd)
	: _pimpl(std::make_unique<ShellImpl>()), _command(cmd)
{
}

Shell::~Shell() = default;

Shell::Shell(const Shell& other)
	: _pimpl(std::make_unique<ShellImpl>(*other._pimpl))
{
}

Shell& Shell::operator=(const Shell& other)
{
	*_pimpl = *other._pimpl;
	return *this;
}

Shell::Shell(Shell&& other) noexcept
	: _pimpl(std::make_unique<ShellImpl>(std::move(*other._pimpl)))
{
}

Shell& Shell::operator=(Shell&& other) noexcept
{
	*_pimpl = std::move(*other._pimpl);
	return *this;
}
