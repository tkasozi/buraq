// MIT License
//
// Copyright (c)  "2025" Talik A. Kasozi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by talik on 4/20/2024.
//

#include <QTextBlock>
#include <QThread>
#include <QString>
#include <QTextStream>
#include <QInputDialog> // For getting parameters for different tasks

#include "EditorMargin.h"
#include "Editor.h"
#include "IconButton.h"
#include "AppUi.h"

EditorMargin::EditorMargin(AppUi *ptrParent) :
		QWidget(ptrParent),
		appUi(ptrParent),
		minion(nullptr),
		workerThread(nullptr) {
	auto layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 4, 8, 0);

	const int playButtonSize = 32;

	QString runBtnStyles(
			"color: #FFF;"
			"border: 0px;"
			"background-color: #232323;"
	);

	runCode_ = std::make_unique<IconButton>(QIcon(ItoolsNS::main_config.getAppIcons().executeIcon),
											playButtonSize, playButtonSize, runBtnStyles);

	// set tooltip for the run buttons
	runCode_->setToolTip(
			"Run code."
			" & "
			"Highlighted code."
	);

	layout->addWidget(runCode_.get());

	connect(runCode_.get(), &IconButton::clicked, this, &EditorMargin::runCode);

	setLayout(layout);
	setFixedWidth(playButtonSize);
	setStyleSheet("border-left: 1px solid #383838;");
}

void EditorMargin::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
}

void EditorMargin::runCode() {
	if (workerThread && workerThread->isRunning()) {
		// Work is already in progress. Please stop current work first.;
		return;
	}

	this->setupWorkerThread();

	QMetaObject::invokeMethod(minion, "doWork", Qt::QueuedConnection,
							  Q_ARG(const std::function<QVariant()>, codeRunnerFunc()));
}

std::function<QVariant()> EditorMargin::codeRunnerFunc() {
	return [this]() -> QVariant {
		// Run selected code
		QString script = appUi->getEditor()->textCursor().selectedText();
		if (script.isEmpty()) {
			// run entire file
			script = appUi->getEditor()->toPlainText();
		}

		if (script.isEmpty()) {
			return {};
		}

		// Removes Paragraph Separator (PS) character
		auto cleaned = script.replace("\u2029", "\n");

		const ProcessedData processedData = appUi->getLangPluginManager()->callPerformAction(
				(void *) cleaned.toStdString().c_str());

		return QVariant::fromValue(processedData.resultValue);
	};
}

void EditorMargin::updateOutputResult(int exitCode, const QString &output, const QString &error) {
	OutputDisplay *outputDisplay = appUi->getOutputDisplay();
	QStatusBar * statusBar = appUi->getQStatusBar();

	outputDisplay->show();

	if (exitCode == 0) {

		outputDisplay->log(output, error);

		statusBar->showMessage(error.isEmpty() ? "Completed!" : "Completed with errors.", 2000);
	} else {
		statusBar->showMessage("Process failed!");
		outputDisplay->log("", error);
	}
}

void EditorMargin::setupWorkerThread() {
	workerThread = new QThread(this);
	workerThread->setObjectName("CodeRunner");

	// 2. Create Minion (no parent initially, will be managed by thread lifecycle)
	minion = new Minion();
	// moves minion's work the workerThead
	minion->moveToThread(workerThread);

	// 4. Connect Signals and Slots
	//    When thread starts, tell minion to start working
	connect(workerThread, &QThread::started, minion, [this]() {
		appUi->getQStatusBar()->showMessage("Executing..");
	});

	// Connect minion signals to MainWindow slots
	connect(minion, &Minion::progressUpdated, this, &EditorMargin::handleProgress);
	connect(minion, &Minion::resultReady, this, &EditorMargin::handleTaskResults);
	connect(minion, &Minion::workFinished, this, &EditorMargin::handleWorkerFinished, Qt::QueuedConnection);

	// When the worker signals it's done with its task
	connect(minion, &Minion::workFinished, workerThread,
			&QThread::quit, Qt::QueuedConnection); // Tell thread to exit its event loop

	// When the thread's event loop has finished (after quit())
	connect(workerThread, &QThread::finished, minion, &QObject::deleteLater, Qt::QueuedConnection);
	connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater,
			Qt::QueuedConnection); // Self-delete thread object

	// 5. Start the thread
	workerThread->start();
}

void EditorMargin::handleTaskResults(const QVariant &result) {
	if (result.isValid() && result.canConvert<std::wstring>()) {
		const auto data = result.value<std::wstring>();
		updateOutputResult(0, QString::fromStdWString(data), "");
	} else {
		updateOutputResult(1, "", "Error failed to execute task.");
	}
}

void EditorMargin::handleProgress(int i) {
	appUi->getOutputDisplay();
	appUi->getQStatusBar()->showMessage("Executing...", 5000);
}

void EditorMargin::handleWorkerFinished() {
	appUi->getOutputDisplay();
	appUi->getQStatusBar()->showMessage("Completed", 5000);

	workerThread = nullptr;
	minion = nullptr;
}
