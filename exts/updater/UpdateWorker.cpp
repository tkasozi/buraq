//
// Created by talik on 6/7/2025.
//

#include "UpdateWorker.h"

#include <QThread>
#include <QFileInfo>

// Include Windows header for process waiting
#ifdef _WIN32

#include <windows.h>

#endif

UpdateWorker::UpdateWorker(QObject* parent) : QObject(parent)
{
}

void UpdateWorker::doUpdate(const QString& installerPath, const QString& installationPath,
                            const unsigned long parentPID)
{
    qDebug() << "UpdateWorker started in thread:" << QThread::currentThread();

    // --- 1. Wait for Main App to Close ---
    waitForMainAppToClose(parentPID);
    emit progressChanged(10); // Update progress after waiting

    // --- 2. Perform Update Tasks ---
    emit statusTextChanged("Applying update...");
    emit logMessage(QString("Update package: %1").arg(installerPath));
    emit logMessage(QString("Installation path: %1").arg(installationPath));

    // extracting files
    if (!installNewVersion(installerPath.toStdString()))
    {
        return;
    }
    emit progressChanged(50);

    emit progressChanged(80);
    emit logMessage("Replacing application files...");
    QThread::sleep(2); // Simulate work
    emit logMessage("File replacement complete.");

    // --- 3. Finish ---
    emit progressChanged(90);
    emit statusTextChanged("Update successful!");
    emit logMessage("Update process finished.");

    // restart app
    emit logMessage("Restarting application...");
    emit progressChanged(100);
    QThread::sleep(1); // User friendliness
    emit finished(true, "Update completed successfully!"); // Signal success
    QThread::sleep(2); // Wait a moment for the OS to release file handles

    const auto mainApp = std::filesystem::path(installationPath.toStdString()) / "buraq.exe";
    // Assuming your app is MyApp.exe
    launchApp(mainApp, "--show-gui");

    emit logMessage(QString("Relaunching %1").arg(QString::fromStdString(mainApp.string())));
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

void UpdateWorker::launchApp(const std::filesystem::path& appPath, const QString& args)
{
    if (appPath == std::filesystem::path())
    {
        qDebug() << "Application should be installed in user AppData path.";
        return;
    }

    emit logMessage(QString("Relaunching %1").arg(QString::fromStdString(appPath.string())));

    auto cmd_string =
        QString::fromStdString(appPath.string());

    if (!args.isEmpty())
    {
        cmd_string = QString(R"("%1" %2)").arg(
            QString::fromStdString(appPath.string()),
            QString(args));
    }

    const auto cmdLine = new char[cmd_string.length() + 1];
    strcpy(cmdLine, cmd_string.toStdString().c_str());

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (
        CreateProcessA(
            args.isEmpty() ? appPath.string().c_str() : NULL,
            args.isEmpty() ? NULL : cmdLine,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            args.isEmpty() ? appPath.parent_path().string().c_str() : NULL,
            &si,
            &pi)
    )
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
    {
        emit logMessage(QString("Failed to relaunch main application. Error: %1 ").arg(
            QString::fromStdString(appPath.string())));
    }
}

// Should remove the old installation and install new version
bool UpdateWorker::installNewVersion(const std::string& installerPath)
{
    emit logMessage("Extracting files...");

    if (!installerPath.ends_with(".exe"))
    {
        emit logMessage(
            QString("Error: The path provided is not a EXE file %1").arg(QString::fromStdString(installerPath)));
        emit finished(false, "Update Failed!"); // Signal success
        return false;
    }

    // Run the installer. Accept all defaults.
    launchApp(installerPath, "/VERYSILENT /SP- /NORESTART");

    emit logMessage("Extraction complete.");
    return true;
}
