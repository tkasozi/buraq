//
// Created by talik on 6/7/2025.
//

#ifndef UPDATE_WORKER_H
#define UPDATE_WORKER_H


#include <QObject>
#include <QString>

class UpdateWorker : public QObject
{
Q_OBJECT

public:
	explicit UpdateWorker(QObject *parent = nullptr);

public slots:
	// The main function to run in the background thread
	void doUpdate(const QString& packagePath, const QString& installPath, unsigned long parentPID);

signals:
	// Signals to update the GUI
	void statusTextChanged(const QString& text);
	void logMessage(const QString& message);
	void progressChanged(int percentage);
	void finished(bool success, const QString& finalMessage);
	void restart();

private:
	void waitForMainAppToClose(unsigned long parentPID);
	void relaunchMainApp(const std::string &installPath);
	bool extractZip(const std::string &zipPath);
};

#endif //UPDATE_WORKER_H
