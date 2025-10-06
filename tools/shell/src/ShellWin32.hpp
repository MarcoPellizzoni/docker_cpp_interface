#include "Shell.h"

#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>


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
        // Create pipe for standard output
        HANDLE hStdoutRead, hStdoutWrite;
        SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0)) {
            StdOut = "";
            StdErr = "Error creating stdout pipe.";
            ExitStatus = -1;
            return;
        }

        // Create pipe for standard error
        HANDLE hStderrRead, hStderrWrite;
        if (!CreatePipe(&hStderrRead, &hStderrWrite, &saAttr, 0)) {
            StdOut = "";
            StdErr = "Error creating stderr pipe.";
            ExitStatus = -1;
            return;
        }

        // Set up the process information
        STARTUPINFO si = { 0 };
        si.cb = sizeof(STARTUPINFO);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hStdoutWrite;
        si.hStdError = hStderrWrite;

        PROCESS_INFORMATION pi = { 0 };

        // Start the process
        if (!CreateProcess(
            NULL,   // Application name (NULL to use command line directly)
            const_cast<char*>(Command.c_str()), // Command line
            NULL,   // Process attributes
            NULL,   // Thread attributes
            TRUE,   // Inherit handles
            0,      // No special creation flags
            NULL,   // Environment (NULL for default)
            NULL,   // Current directory (NULL for default)
            &si,    // Startup info
            &pi     // Process information
        )) 
        {
            StdOut = "";
            StdErr = "Error creating process.";
            ExitStatus = -1;
            return;
        }

        // Close the write ends of the pipes (not needed after the process starts)
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrWrite);


        // Read standard output from the pipe
        std::string stdout_str, stderr_str;
        DWORD exit_code = 0;

        char buffer[4096];
        DWORD bytesRead;
        while (ReadFile(hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the string
            stdout_str += buffer;
        }

        // Read standard error from the pipe
        while (ReadFile(hStderrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';  // Null-terminate the string
            stderr_str += buffer;
        }

        // Wait for the process to finish
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Get exit code
        if (!GetExitCodeProcess(pi.hProcess, &exit_code))
        {
            StdOut = "";
            StdErr = "Fail to get exit code.";
            ExitStatus = -1;
            return;
        }

        StdOut = stdout_str;
        StdErr = stderr_str;
        ExitStatus = exit_code;

        // Clean up handles
        CloseHandle(hStdoutRead);
        CloseHandle(hStderrRead);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return;
    }
};
