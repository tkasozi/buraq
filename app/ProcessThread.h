//
// Created by talik on 4/30/2024.
//

#ifndef IT_TOOLS_PROCESSTHREAD_H
#define IT_TOOLS_PROCESSTHREAD_H

#include <QProcess>
#include <QThread>
#include <QMutex>

class ProcessThread : public QThread {
Q_OBJECT

protected:
signals:

	void processStarted();
	void processFinished(int exitCode, const QString &output, const QString &error);

public:
	explicit ProcessThread(QString, QStringList &);

	ProcessThread() = default;

	~ProcessThread() override = default;

private slots:

	void run() override;

private:
	QString program;
	QStringList arguments;
};


#endif //IT_TOOLS_PROCESSTHREAD_H
