//
// Created by talik on 4/30/2024.
//

#include <utility>

#include "ProcessThread.h"


void ProcessThread::run() {
	QProcess process;
	process.setProgram(program);
	process.setArguments(arguments);

	// Indicates process has started.
	emit processStarted();

	QString output;
	QMutex outputMutex;

	QObject::connect(&process, &QProcess::readyReadStandardOutput, [&process, &output, &outputMutex]() {
		outputMutex.lock();
		output.append(process.readAllStandardOutput());
		outputMutex.unlock();
	});

	QString error;
	QMutex errorMutex;
	QObject::connect(&process, &QProcess::readyReadStandardError, [&process, &error, &errorMutex]() {
		errorMutex.lock();
		error.append(process.readAllStandardError());
		errorMutex.unlock();
	});

	process.start();
	process.waitForFinished();

	int exitCode = process.exitStatus();

	emit processFinished(exitCode, output, error);
}

ProcessThread::ProcessThread(QString programName, QStringList &args)
		: QThread(), program(std::move(programName)), arguments(args) {}
