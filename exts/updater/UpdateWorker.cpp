//
// Created by talik on 6/7/2025.
//

#include "UpdateWorker.h"

#include <QThread>
#include <QFileInfo>

// Include Windows header for process waiting
#ifdef _WIN32

#include <windows.h>
#include <unzip.h>

#endif

UpdateWorker::UpdateWorker(QObject *parent) : QObject(parent) {
}

void UpdateWorker::doUpdate(const QString &packagePath, const QString &installPath, unsigned long parentPID) {
	qDebug() << "UpdateWorker started in thread:" << QThread::currentThread();

	// --- 1. Wait for Main App to Close ---
	waitForMainAppToClose(parentPID);
	emit progressChanged(10); // Update progress after waiting

	// --- 2. Perform Update Tasks ---
	emit statusTextChanged("Applying update...");
	emit logMessage(QString("Update package: %1").arg(packagePath));
	emit logMessage(QString("Installation path: %1").arg(installPath));

	// extracting files
	if (!extractZip(packagePath.toStdString())) {
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
	relaunchMainApp(installPath.toStdString());

	// In a real scenario, you'd have error handling:
	// if (an_error_occurred) {
	//     emit finished(false, "An error occurred during the update.");
	// }
}

void UpdateWorker::waitForMainAppToClose(unsigned long parentPID) {
#ifdef _WIN32
	emit statusTextChanged("Waiting for main application to close...");
	emit logMessage(QString("Watching Parent Process ID: %1").arg(parentPID));

	HANDLE hParentProcess = OpenProcess(SYNCHRONIZE, FALSE, parentPID);
	if (hParentProcess == NULL) {
		// Could be that the parent already closed, which is fine.
		emit logMessage("Parent process handle could not be opened, assuming it already exited.");
	} else {
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

void UpdateWorker::relaunchMainApp(const std::string &installPath) {
	std::filesystem::path mainAppPath =
			std::filesystem::path(installPath) / "bin" / "ITools.exe"; // Assuming your app is MyApp.exe
	emit logMessage(QString("Relaunching %1").arg(QString::fromStdString(mainAppPath.string())));

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (
			CreateProcessA(
					mainAppPath.string().c_str(),
					NULL,
					NULL,
					NULL,
					FALSE,
					0,
					NULL,
					installPath.c_str(),
					&si,
					&pi)
			) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		emit logMessage(QString("Main application relaunched."));
		emit restart();
	} else {
		emit logMessage(QString("Failed to relaunch main application. Error: %1 ").arg(
				QString::fromStdString(mainAppPath.string())));
	}
}

// You'll need a ZIP extraction library, e.g., Minizip or a system call to a utility
bool UpdateWorker::extractZip(const std::string &zipFilepath) {
	emit logMessage("Extracting files...");

	if (!zipFilepath.ends_with(".zip")) {
		emit logMessage(QString("Error: The path provided is not a zip file %1").arg(QString::fromStdString(zipFilepath)));
		emit finished(false, "Update Failed!"); // Signal success
		return false;
	}

	// const std::string extractToPath = ""; // generate extraction path
	// --- 1. Open the ZIP archive ---
	unzFile zf = unzOpen(zipFilepath.c_str());
	if (zf == NULL) {
		emit logMessage(QString("Error: Could not open ZIP file %1").arg(QString::fromStdString(zipFilepath)));
		return false;
	}

	emit logMessage("Extraction complete.");
	return true;
}