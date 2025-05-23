//
// Created by talik on 4/20/2024.
//

#ifndef IT_TOOLS_EDITOR_MARGIN_H
#define IT_TOOLS_EDITOR_MARGIN_H

#include <QWidget>
#include <QThread>
#include <QPlainTextEdit>
#include "CustomLabel.h"
#include "Editor.h"
#include "IconButton.h"
#include "AppUi.h"
#include "../utils/Minion.h"

class EditorMargin : public QWidget {
Q_OBJECT

public slots:

	void handleTaskResults(const QVariant &result); // Modified to take QVariant
	void handleProgress(int);

private slots:

	void runCode();

	void handleWorkerFinished();

	void updateOutputResult(int exitCode, const QString &output, const QString &error);

signals:

	void processStarted();

public:
	explicit EditorMargin(AppUi *ptrParent);

	~EditorMargin() override {
		if (workerThread && workerThread->isRunning()) {
			workerThread->requestInterruption();
			workerThread->quit(); // Ask event loop to quit
			if (!workerThread->wait(5000)) { // Wait for max 5 seconds
				workerThread->terminate(); // Force terminate (last resort)
				workerThread->wait();      // Wait for termination
			}
		}
	};

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	// smart pointer will be cleaned up
	std::unique_ptr<IconButton> runCode_;
	std::unique_ptr<AppUi> appUi;

	// cleanup will be handled by  &QObject::deleteLater
	QThread *workerThread;
	Minion *minion;

	void setupWorkerThread();

	std::function<QVariant()> codeRunnerFunc();
};


#endif //IT_TOOLS_EDITOR_MARGIN_H
