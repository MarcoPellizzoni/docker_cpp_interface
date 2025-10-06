#pragma once

#if defined(WIN32) && defined(_BUILD_SHELL_LIB_DLL)
	#if defined(_SHELL_LIB_EXPORT)
		#define SHELLAPI __declspec(dllexport)
	#else
		#define SHELLAPI __declspec(dllimport)
	#endif
#else
	#define SHELLAPI
#endif

#include <string>
#include <memory>
#include <stdexcept>

class SHELLAPI Shell
{
public:

	enum Exit
	{
		SUCCESS = 0,
		FAIL,
	};

	struct Output2
	{
		Exit		exitCode;
		std::string result;
		friend bool operator==(Output2 o, Exit code)
		{
			return o.exitCode == code;
		}
		friend bool operator!=(Output2 o, Exit code)
		{
			return o.exitCode != code;
		}
	};

	typedef std::string                 Input;
	typedef std::pair<int, std::string> Output;


	Shell();
	Shell(Input cmd);
	virtual ~Shell();
	Shell(const Shell&);
	Shell& operator=(const Shell&);
	Shell(Shell&&) noexcept;
	Shell& operator=(Shell&&) noexcept;

	/**
	 * @brief   Executes the command previously set and returns the stdout result of the executed command.
	 *          The Exit inspect_status can be read using getExitStatus().
	 * @return  The result of the command as a ShellOutput type.
	 */
	Output execute();

	/**
	 * @brief   Executes a command passed as a string parameter and returns the stdout string result of the executed command.
	 *          The exit inspect_status is set int the second parameter.
	 *          It does not save the results.
	 * @param   command: the command to execute
	 * @return  The result of the command as a string and the exit inspect_status as an integer
	 */
	Output execute(const Input commandS);

	/**
		@brief  Immediatly executes a give command. Do not hold the command and the result.
				No need to create instance of the class.
		@param  command - the command to execute
		@retval         - the result
	**/
	static Output2 prompt(const Input command);

	void setCommand(const Input cmd) noexcept;

	Input getCommand() const noexcept;

	int getExitStatus() const noexcept;
	std::string getResult() const noexcept;

protected:
	
	int         _exit_status = -1;
	std::string _result;
	Input 		_command;

	struct ShellImpl;
	std::unique_ptr<ShellImpl> _pimpl;
};

