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
#include "CommonWidget.h"

class EditorActionArea : public CommonWidget {
Q_OBJECT

private slots:
	void handleProgress(int);

	void handleTaskResults(const QVariant &result); // Modified to take QVariant

	void runCode();

	void handleWorkerFinished();

	void updateOutputResult(int exitCode, const QString &output, const QString &error);

public:
	explicit EditorActionArea(QWidget *appUi);

	~EditorActionArea() override;

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	// smart pointer will be cleaned up
	std::unique_ptr<IconButton> runCode_;

	// should be managed elsewhere
	QWidget *appUi;

	// cleanup will be handled by  &QObject::deleteLater
	QThread *workerThread;
	Minion *minion;

	void setupWorkerThread();
	void setupSignals() const override;
};


#endif //IT_TOOLS_EDITOR_MARGIN_H
