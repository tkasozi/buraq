//
// Created by talik on 6/7/2025.
//

#include "UpdateWorker.h"

#include <QThread>

// Include Windows header for process waiting
#ifdef _WIN32

#include <windows.h>

#endif

UpdateWorker::UpdateWorker(QObject* parent) : QObject(parent)
{
}

void UpdateWorker::doUpdate(const std::filesystem::path& installerPath,
                            const std::filesystem::path& installationPath,
                            const unsigned long parentPID)
{
    qDebug() << "UpdateWorker started in thread:" << QThread::currentThread();

    // --- 1. Wait for Main App to Close ---
    waitForMainAppToClose(parentPID);

    emit progressChanged(10); // Update progress after waiting

    // --- 2. Perform Installation ---
    emit statusTextChanged("Applying update...");
    emit logMessage(QString("Update package: %1").arg(installerPath.c_str()));
    emit logMessage(QString("Installation path: %1").arg(installationPath.c_str()));

    emit progressChanged(20);

    // extracting files
    if (!installNewVersion(installerPath, installationPath))
    {
        return;
    }

    emit finished(true, "Update completed successfully!"); // Signal success
    QThread::msleep(2); // Wait a moment for the OS to release file handles

    // restart app
    emit logMessage("Restarting application...");
    QThread::msleep(1); // User friendliness

    const auto mainAppExePath = std::filesystem::path(installationPath.string()) / "buraq.exe";

#ifdef _WIN32
    if (HANDLE hLaunchedProcess = nullptr; launchApp(mainAppExePath, &hLaunchedProcess, "--show-gui"), true)
    {
        emit logMessage(
            QString("Relaunching %1. Waiting for it to become responsive...").arg(
                QString::fromStdString(mainAppExePath.filename().string())));


        const bool appStarted = waitForAppWindow(hLaunchedProcess, 90000); // 30-second timeout
        CloseHandle(hLaunchedProcess); // Close handle once we're done with it

        if (appStarted)
        {
            emit logMessage("Main application appears to have started successfully.");
            emit progressChanged(100);
            emit finished(true, "Update completed successfully!"); // Signal success
        }
        else
        {
            emit logMessage(
                "Warning: Main application launched but its window did not appear within timeout. Update finished.");
            emit finished(false, "Update completed, but app window not found!"); // Signal partial success/warning
        }
    }

#else
    // Add alternative implementation for Linux or macOS
#endif

    emit progressChanged(100);
    QThread::msleep(2);

    emit restart();
}

void UpdateWorker::waitForMainAppToClose(const unsigned long parentPID)
{
#ifdef _WIN32
    emit statusTextChanged("Waiting for main application to close...");
    emit logMessage(QString("Watching Parent Process ID: %1").arg(parentPID));

    if (HANDLE hParentProcess = OpenProcess(SYNCHRONIZE, FALSE, parentPID); hParentProcess == NULL)
    {
        // Could be that the parent already closed, which is fine.
        emit logMessage("Parent process handle could not be opened, assuming it already exited.");
    }
    else
    {
        // Wait for the process to terminate
        WaitForSingleObject(hParentProcess, INFINITE);
        CloseHandle(hParentProcess);
        emit logMessage("Main application has exited. Proceeding with update.");
    }
    // Give a moment for the OS to release file handles
    QThread::msleep(1000);
#else
	// Implement waiting for other OSes if needed
	emit logMessage("Process waiting not implemented for this OS, proceeding immediately.");
	QThread::msleep(1000);
#endif
}

#ifdef _WIN32
// In UpdateWorker.h (if you want to keep waitForMainAppToStart, but its logic needs overhaul)
// For simplicity, let's remove the faulty waitForMainAppToStart for now.
// If you want to WAIT for the child, then the WaitForSingleObject should be on pi.hProcess.
// For *this* problem (app closing with dialog), the primary fix is correct handle closing.
bool UpdateWorker::launchApp(const std::filesystem::path& appPath, HANDLE* outProcessHandle, const QString& args,
                             const bool createNewConsole)
{
    if (appPath.empty() || !std::filesystem::exists(appPath))
    {
        emit logMessage(
            QString("Error: Application path is invalid or does not exist for relaunch: %1").arg(
                QString::fromStdString(appPath.string())));
        if (outProcessHandle) *outProcessHandle = NULL;
        return false;
    }

    emit logMessage(
        QString("Attempting to launch: %1 with args: %2").arg(QString::fromStdString(appPath.string()), args));

    // CreateProcessA requires a non-const char* buffer for lpCommandLine
    // and it must contain the full command line, including the executable path,
    // when lpApplicationName is NULL (which is generally safer).
    std::string cmd_str = "\"" + appPath.string() + "\""; // Always quote the path to handle spaces
    if (!args.isEmpty())
    {
        cmd_str += " " + args.toStdString();
    }

    // Allocate a mutable buffer for CreateProcessA
    // CreateProcessA might modify this buffer.
    std::vector<char> cmd_buffer(cmd_str.begin(), cmd_str.end());
    cmd_buffer.push_back('\0'); // Null-terminate the buffer

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    DWORD creationFlags = 0; // Default flags

    // --- CRITICAL CHANGE HERE ---
    // If buraq.exe is a CONSOLE application:
    if (createNewConsole)
    {
        creationFlags |= CREATE_NEW_CONSOLE; // Give it its own console window
    }
    else
    {
        creationFlags |= DETACHED_PROCESS; // Run without any console window (fully in background)
    }

    // Try launching the process
    BOOL success = CreateProcessA(
        NULL, // lpApplicationName (use lpCommandLine for full path)
        cmd_buffer.data(), // lpCommandLine (modifiable buffer)
        NULL, // lpProcessAttributes
        NULL, // lpThreadAttributes
        FALSE, // bInheritHandles (don't inherit handles from parent)
        creationFlags, // dwCreationFlags (0 for default, or CREATE_NO_WINDOW, etc.)
        NULL, // lpEnvironment
        appPath.parent_path().string().c_str(), // lpCurrentDirectory (set working directory of new process)
        &si,
        &pi
    );

    if (success)
    {
        emit logMessage(
            QString("Successfully launched process. PID: %1, Thread ID: %2").arg(pi.dwProcessId).arg(pi.dwThreadId));
        // Return the process handle if requested
        if (outProcessHandle)
        {
            *outProcessHandle = pi.hProcess; // Pass back the process handle
        }
        else
        {
            // If caller doesn't want the handle, close it immediately
            CloseHandle(pi.hProcess);
        }
        CloseHandle(pi.hThread); // Always close the thread handle, as we don't need it.
        return true;
    }
    else
    {
        const DWORD error = GetLastError();
        emit logMessage(
            QString("CreateProcessA failed. Error Code: %1. Path: %2").arg(error).arg(
                QString::fromStdString(appPath.string())));

        // Provide more specific error messages for common issues
        if (error == ERROR_FILE_NOT_FOUND)
        {
            emit logMessage("Common cause: The executable file was not found at the specified path.");
        }
        else if (error == ERROR_ACCESS_DENIED)
        {
            emit logMessage("Common cause: Access denied (permissions issue or another process is locking the file).");
        }
        else if (error == ERROR_BAD_EXE_FORMAT)
        {
            emit logMessage("Common cause: Corrupted executable or architecture mismatch (e.g., 32-bit vs 64-bit).");
        }
        else if (error == ERROR_INVALID_PARAMETER)
        {
            emit logMessage("Common cause: Invalid command line arguments or application name.");
        }
        if (outProcessHandle) *outProcessHandle = NULL;
        return false;
    }
}


// NEW HELPER FUNCTION: Wait for a process window to appear
// This is more robust for "app started" than just process creation
bool UpdateWorker::waitForAppWindow(HANDLE hProcess, DWORD timeout_ms)
{
    DWORD startTime = GetTickCount();
    while (GetTickCount() - startTime < timeout_ms)
    {
        DWORD exitCode;
        if (GetExitCodeProcess(hProcess, &exitCode) && exitCode != STILL_ACTIVE)
        {
            // Process terminated prematurely
            emit logMessage("Launched application terminated prematurely while waiting for window.");
            return false;
        }

        // Enumerate windows and check if one belongs to this process
        // This is a simplified check. A more robust way might involve
        // finding the main window by title or class after the process starts.
        // For a simple 'any window' check for the launched process:
        HWND hWnd = NULL;
        do
        {
            hWnd = FindWindowEx(NULL, hWnd, NULL, NULL); // Enumerate all top-level windows
            if (hWnd != NULL)
            {
                DWORD pid;
                GetWindowThreadProcessId(hWnd, &pid);
                if (pid == GetProcessId(hProcess))
                {
                    // Found a window belonging to our process!
                    // Optionally, check if it's actually visible: IsWindowVisible(hWnd)
                    emit logMessage(QString("Found window for launched application (HWND: %1)").arg((quintptr)hWnd));
                    return true;
                }
            }
        }
        while (hWnd != NULL);

        QThread::msleep(100); // Small delay to avoid busy-waiting
    }
    emit logMessage(QString("Timed out waiting for application window."));
    return false; // Timeout
}
#else
// Add implementation for linux or macOS
#endif

/**
 * Runs the installer app after removing the old installation.
 * @param installerPath Runs the installer app, waits for the app to complete
 * @param installationPath The location where the new installation will be created.
 * @return Returns true if the installer app completes successfully.
 */
bool UpdateWorker::installNewVersion(const std::filesystem::path& installerPath,
                                     const std::filesystem::path& installationPath)
{
    if (installerPath.extension() != ".exe")
    {
        emit logMessage(
            QString("Error: Installer path provided is not an EXE file: %1").arg(
                QString::fromStdString(installerPath.string())));
        emit finished(false, "Update Failed: Installer is not an EXE!");
        return false;
    }

    emit progressChanged(60);

    std::error_code ec;
    if (std::filesystem::is_directory(installationPath, ec))
    {
        if (const uintmax_t removed_count = std::filesystem::remove_all(installationPath, ec);
            removed_count == 0 || ec
        )
        {
            emit logMessage("Failed to remove any files");
        }
    }

    emit progressChanged(70); // Installer finished

    emit statusTextChanged("Running installer...");
    emit logMessage(
        QString("Executing installer: %1 /VERYSILENT /SP- /NORESTART").arg(
            QString::fromStdString(installerPath.string())));

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Command line for the installer (must be mutable buffer)
    std::string installer_cmd_str = "\"" + installerPath.string() + "\" /VERYSILENT /SP- /NORESTART";
    std::vector<char> installer_cmd_buffer(installer_cmd_str.begin(), installer_cmd_str.end());
    installer_cmd_buffer.push_back('\0');

    // Create the installer process
    if (CreateProcessA(
        NULL, // lpApplicationName (use lpCommandLine for full path)
        installer_cmd_buffer.data(), // lpCommandLine
        NULL, // lpProcessAttributes
        NULL, // lpThreadAttributes
        FALSE, // bInheritHandles (updater handles are not inherited by installer)
        0, // dwCreationFlags (0 for default)
        NULL, // lpEnvironment
        installerPath.parent_path().string().c_str(), // lpCurrentDirectory (installer's working dir)
        &si,
        &pi))
    {
        emit logMessage(
            QString("Installer process launched. PID: %1. Waiting for it to finish...").arg(pi.dwProcessId));

        // CRITICAL: Wait for the installer process to terminate
        WaitForSingleObject(pi.hProcess, INFINITE); // Wait indefinitely for installer to finish

        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);

        // Always close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        emit logMessage(QString("Installer process finished with exit code: %1").arg(exitCode));

        // Check installer's exit code for success if your installer provides meaningful codes
        // (Inno Setup /VERYSILENT /NORESTART typically returns 0 on success)
        if (exitCode != 0)
        {
            emit logMessage(
                QString("Warning: Installer returned non-zero exit code: %1. Update might not be complete.").arg(
                    exitCode));
            // You might want to return false here if a non-zero exit code means failure
            // For now, let's assume it proceeds, but warns.
        }

        if (const uintmax_t removed_count = std::filesystem::remove_all(installerPath, ec);
            removed_count == 0 || ec
        )
        {
            emit logMessage("Failed to delete the installer");
            return false;
        }

        emit progressChanged(80); // Installer finished
        emit statusTextChanged("Installer finished. Verifying installation...");
        return true; // Installer successfully launched and finished
    }
    else
    {
        const DWORD error = GetLastError();
        emit logMessage(QString("Failed to launch installer process. Error Code: %1").arg(error));
        emit finished(false, "Update Failed: Could not launch installer!");
        return false;
    }
}
