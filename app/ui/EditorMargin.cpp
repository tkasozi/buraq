//
// Created by talik on 4/20/2024.
//

#include <QTextBlock>
#include <QString>
#include <QTextStream>

#include "EditorMargin.h"
#include "Editor.h"
#include "IconButton.h"
#include "AppUi.h"

#include "../ProcessThread.h"

EditorMargin::EditorMargin(AppUi *ptrParent) : QWidget(ptrParent), ptrParent(ptrParent) {
	auto layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 4, 8, 0);

	const int playButtonSize = 32;
	QString runBtnStyles("color: #FFF;"
						 "border: 0px;"
						 "background-color: #232323;");

	exeScript = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons().executeIcon),
							   playButtonSize, playButtonSize, runBtnStyles);

	exeSelectedScript = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons().executeSelectedIcon),
									   playButtonSize, playButtonSize, runBtnStyles);

	// set tooltip for the run buttons
	exeScript->setToolTip("Execute the whole script");
	exeSelectedScript->setToolTip("Execute selected/highlighted script");

	layout->addWidget(exeScript);
	layout->addWidget(exeSelectedScript);

	connect(exeScript, &IconButton::clicked, this, &EditorMargin::onExecuteWholeScriptButtonClicked);
	connect(exeSelectedScript, &IconButton::clicked, this, &EditorMargin::onExecuteSelectedScriptButtonClicked);

	setLayout(layout);
	setFixedWidth(playButtonSize);
	setStyleSheet("border-left: 1px solid #383838;");
}

void EditorMargin::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
}


void EditorMargin::onExecuteWholeScriptButtonClicked() {
	Editor *editor = ptrParent->getEditor();

	// Get the current script from the editor
	QString script = editor->toPlainText();

	onExecuteScriptButtonClicked(script);
}

void EditorMargin::onExecuteSelectedScriptButtonClicked() {
	Editor *editor = ptrParent->getEditor();
	QString selectedScript = editor->textCursor().selectedText();

	if (!selectedScript.isEmpty()) {
		// Removes Paragraph Separator (PS) character
		auto cleaned = selectedScript.replace("\u2029", "\n");
		// qDebug() << cleaned;
		onExecuteScriptButtonClicked(cleaned);
	}
}

void EditorMargin::onExecuteScriptButtonClicked(QString &script) const {
	// Define arguments for powershell.exe
	QStringList arguments;
	arguments << "-ExecutionPolicy" << "Bypass";  // Be cautious with security implications!
	arguments << "-Command";
	arguments << script;

	// Create a QProcess object
	auto *thread = new ProcessThread(ItoolsNS::main_config.getPowershellPath(), arguments);

	connect(thread, &ProcessThread::processStarted, this, &EditorMargin::onProcessStarted);
	connect(thread, &ProcessThread::processFinished, this, &EditorMargin::updateOutputResult);

	thread->start();
}

void EditorMargin::onProcessStarted() {
	QStatusBar * statusBar = ptrParent->getQStatusBar();
	statusBar->showMessage("Executing.. ", 2000);
}

void EditorMargin::updateOutputResult(int exitCode, const QString &output, const QString &error) {
	OutputDisplay *outputDisplay = ptrParent->getOutputDisplay();
	QStatusBar * statusBar = ptrParent->getQStatusBar();

	outputDisplay->show();

	if (exitCode == 0) {

		outputDisplay->log(output, error);

		statusBar->showMessage(error.isEmpty() ? "Done!" : "Completed with errors.", 2000);
	} else {
		statusBar->showMessage("Process failed!");
		outputDisplay->log("", error);
	}
}