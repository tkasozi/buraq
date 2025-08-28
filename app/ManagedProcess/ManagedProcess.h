//
// Created by talik on 8/28/2025.
//

#ifndef MAINPROJECT_PS_H
#define MAINPROJECT_PS_H

#include <windows.h>
#include <filesystem>
#include <string>
#include <iostream>

class ManagedProcess
{
public:
    // Constructor: Launches the process
    explicit ManagedProcess(const std::filesystem::path& executablePath)
    {
        // Ensure the process isn't marked as running initially
        m_isRunning = false;

        std::wstring commandLine = executablePath.wstring();

        STARTUPINFOW si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&m_processInfo, sizeof(m_processInfo));

        // The creation flag to run the process without a console

        // Launch the process
        if (constexpr DWORD creationFlags = CREATE_NO_WINDOW;
            !CreateProcessW(NULL, commandLine.data(), NULL,
                            NULL, FALSE, creationFlags, NULL,
                            NULL, &si, &m_processInfo))
        {
            std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
            return;
        }

        // Wait briefly for the process to initialize
        WaitForInputIdle(m_processInfo.hProcess, 5000); // 5-second timeout

        std::cout << "Process " << executablePath.filename() << " started with PID: " << m_processInfo.dwProcessId <<
            std::endl;
        m_isRunning = true;
    }

    // Destructor: Terminates the process
    ~ManagedProcess()
    {
        if (m_isRunning)
        {
            std::cout << "Terminating process with PID: " << m_processInfo.dwProcessId << std::endl;

            // Forcefully terminate the process
            TerminateProcess(m_processInfo.hProcess, 1); // 1 indicates an abnormal termination

            // Clean up the process and thread handles
            CloseHandle(m_processInfo.hProcess);
            CloseHandle(m_processInfo.hThread);
        }
    }

    // A helper to check if the process was launched successfully
    bool isRunning() const
    {
        return m_isRunning;
    }

private:
    PROCESS_INFORMATION m_processInfo;
    bool m_isRunning;
};

#endif //MAINPROJECT_PS_H
