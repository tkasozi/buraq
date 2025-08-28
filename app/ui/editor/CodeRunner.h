//
// Created by talik on 5/28/2025.
//

#ifndef CODERUNNER_H
#define CODERUNNER_H

#include <QWidget>
#include "IconButton.h"
#include "Minion.h"
#include "../clients/PSClient/PSClient.h"

class PSClient;

class CodeRunner : public IconButton {

Q_OBJECT

private slots:

	void handleProgress(int);

	void handleTaskResults(const QVariant &result); // Modified to take QVariant

	void runCode();

	void handleWorkerFinished();

signals:
	void statusUpdate(QString status, int timeout = 10000);
	void updateOutputResult(int exitCode, const QString &output, const QString &error);

public:
	explicit CodeRunner(QWidget *appUi);
	~CodeRunner() override;

private:

	// should be managed elsewhere
	QWidget *appUi;

	// cleanup will be handled by  &QObject::deleteLater

	PSClient* m_psClient;
	QThread *m_workerThread;
	Minion *m_minion;

	void setupWorker();

	void setupSignals();
};


#endif //CODERUNNER_H
