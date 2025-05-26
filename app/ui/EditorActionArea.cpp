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

#include "EditorActionArea.h"
#include "Editor.h"
#include "IconButton.h"
#include "AppUi.h"

EditorActionArea::EditorActionArea(QWidget *appUi) :
		CommonWidget(appUi),
		appUi(appUi),
		minion(nullptr),
		workerThread(nullptr) {

	const int playButtonSize = 32;

	QString runBtnStyles(
			"color: #FFF;"
			"border: 0px;"
			"background-color: #232323;"
	);

	runCode_ = std::make_unique<IconButton>(QIcon(ItoolsNS::main_config.getAppIcons()->executeIcon),
											playButtonSize, playButtonSize, runBtnStyles);

	// set tooltip for the run buttons
	runCode_->setToolTip(
			"Run code."
			" & "
			"Highlighted code."
	);

	auto layout = new QVBoxLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 4, 8, 0);
	layout->addWidget(runCode_.get());

	setLayout(layout);
	setFixedWidth(playButtonSize);
	setStyleSheet("border-left: 1px solid #383838;");

	// setup signals & slots connection
	EditorActionArea::setupSignals();
}

void EditorActionArea::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
}

void EditorActionArea::runCode() {
	if (workerThread && workerThread->isRunning()) {
		// Work is already in progress. Please stop current work first.;
		return;
	}

	this->setupWorkerThread();

	std::function<QVariant()> task =  [this]() -> QVariant {
		auto *appUi_ = dynamic_cast<AppUi *>(appUi);

		if(!appUi_->getEditor()) {
			// editor must be defined
			return {};
		}

		// Run selected code
		QString script = appUi_->getEditor()->textCursor().selectedText();
		if (script.isEmpty()) {
			// run entire file
			script = appUi_->getEditor()->toPlainText();
		}

		if (script.isEmpty()) {
			return {};
		}

		// Removes Paragraph Separator (PS) character
		auto cleaned = script.replace("\u2029", "\n");

		if(!appUi_->getLangPluginManager()) {
			// LangPluginManager must be defined
			return {};
		}
		const ProcessedData processedData = appUi_->getLangPluginManager()->callPerformAction(
				(void *) cleaned.toStdString().c_str());

		return QVariant::fromValue(processedData.resultValue);
	};

	QMetaObject::invokeMethod(minion, "doWork", Qt::QueuedConnection, Q_ARG(const std::function<QVariant()>, task));
}

void EditorActionArea::setupWorkerThread() {
	workerThread = new QThread(this);
	workerThread->setObjectName("CodeRunner");

	// 2. Create Minion (no parent initially, will be managed by thread lifecycle)
	minion = new Minion();
	// moves minion's work the workerThead
	minion->moveToThread(workerThread);

	// 4. Connect Signals and Slots
	//    When thread starts, tell minion to start working
	connect(workerThread, &QThread::started, minion, [this]() {
		emit statusUpdate("Executing..");
	});

	// Connect minion signals to MainWindow slots
	connect(minion, &Minion::progressUpdated, this, &EditorActionArea::handleProgress);
	connect(minion, &Minion::resultReady, this, &EditorActionArea::handleTaskResults);
	connect(minion, &Minion::workFinished, this, &EditorActionArea::handleWorkerFinished, Qt::QueuedConnection);

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

void EditorActionArea::handleTaskResults(const QVariant &result) {
	if (result.isValid() && result.canConvert<std::wstring>()) {
		const auto data = result.value<std::wstring>();
		QString error = "", resultString = QString::fromStdWString(data);
		int statusCode = 0;
		if (!resultString.isEmpty() && resultString.contains("exception", Qt::CaseInsensitive)) {
			error = resultString;
			resultString = "";
			statusCode = 1;
		}
		emit updateOutputResult(statusCode, resultString, error);
	} else {
		emit updateOutputResult(1, "", "Error failed to execute task.");
	}
}

void EditorActionArea::handleProgress(int i) {
	emit statusUpdate("Executing...");
}

void EditorActionArea::handleWorkerFinished() {
	emit statusUpdate("Completed");

	workerThread = nullptr;
	minion = nullptr;
}

EditorActionArea::~EditorActionArea() {
	// smart pointers are deleted automatically
	// editor pointer should be deleted elsewhere
	appUi = nullptr;

	if (workerThread && workerThread->isRunning()) {
		workerThread->requestInterruption();
		workerThread->quit(); // Ask event loop to quit
		if (!workerThread->wait(5000)) { // Wait for max 5 seconds
			workerThread->terminate(); // Force terminate (last resort)
			workerThread->wait();      // Wait for termination
		}
	}
}

void EditorActionArea::setupSignals() const {
	// Signal to execute the code
	connect(runCode_.get(), &IconButton::clicked, this, &EditorActionArea::runCode);

	auto *appUi_ = dynamic_cast<AppUi *>(appUi);
	// Signal to update status bar in AppUI component for the running process
	connect(this, &EditorActionArea::statusUpdate, appUi_, &AppUi::processStatusSlot);

	// Signal to update the out component in AppUI component for the completed process
	connect(this, &EditorActionArea::updateOutputResult, appUi_, &AppUi::processResultSlot);
}
