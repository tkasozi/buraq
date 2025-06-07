// Updater.exe - main.cpp
#include <windows.h>
#include <string>
#include <iostream>
//#include <tlhelp32.h> // For process checking (optional, better to use Handle)
#include <filesystem> // C++17
#include <QApplication>
#include "../../include/IToolsAPI.h"

#include "UpdaterProgressDialog.h"
#include "UpdateWorker.h"

#include <QThread>

void log(const std::string &_log) {
	db_log("[Updater.exe] " + _log);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// --- Get arguments from command line ---
	if (QApplication::arguments().size() < 4) {
		qCritical() << "Usage: Updater.exe <packagePath> <installPath> <parentPID>";
		return 1;
	}
	QString installer = QApplication::arguments().at(0);
	QString packagePath =  QApplication::arguments().at(1);
	QString installPath =  QApplication::arguments().at(2);
	unsigned long parentPID =  QApplication::arguments().at(3).toULong();

	log("Updater started.");
	log("Installer: " + installer.toStdString());
	log("Package: " + packagePath.toStdString());
	log("Install Dir: " + installPath.toStdString());
	log("Parent PID: " + std::to_string(parentPID));

	// --- Setup Dialog and Worker Thread ---
	UpdateProgressDialog dialog;

	auto* workerThread = new QThread();
	auto* worker = new UpdateWorker();

	worker->moveToThread(workerThread);

	// --- Connect Signals and Slots ---
	// Connect worker signals to dialog slots
	QObject::connect(worker, &UpdateWorker::statusTextChanged, &dialog, &UpdateProgressDialog::setStatusText);
	QObject::connect(worker, &UpdateWorker::logMessage, &dialog, &UpdateProgressDialog::addLogMessage);
	QObject::connect(worker, &UpdateWorker::progressChanged, &dialog, &UpdateProgressDialog::setProgress);
	QObject::connect(worker, &UpdateWorker::finished, &dialog, &UpdateProgressDialog::onUpdateFinished);
	QObject::connect(worker, &UpdateWorker::restart, &dialog, &UpdateProgressDialog::accept);

	// When the worker is finished, quit the thread
	QObject::connect(worker, &UpdateWorker::finished, workerThread, &QThread::quit);
	// When the thread quits, clean up the worker and thread objects
	QObject::connect(workerThread, &QThread::finished, worker, &UpdateWorker::deleteLater);
	QObject::connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);

	// When the thread starts, call the worker's doUpdate function
	QObject::connect(workerThread, &QThread::started, worker, [=]() {
		worker->doUpdate(packagePath, installPath, parentPID);
	});

	// --- Start the Process ---
	workerThread->start(); // Start the background thread

	// Show the dialog modal. The program will stay here until the dialog is closed.
	dialog.exec();

	// The application will quit when dialog.exec() returns
	return 0;
}
