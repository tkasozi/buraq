
//
// Created by talik on 5/28/2025.
//

#include <QIcon>
#include "CodeRunner.h"
#include <QThread>
#include "CustomLabel.h"
#include "IconButton.h"
#include "app_ui/AppUi.h"

CodeRunner::CodeRunner(QWidget *appUi) :
		IconButton(nullptr),
		appUi(appUi),
		minion(nullptr),
		workerThread(nullptr){

	setIcon(QIcon(ItoolsNS::main_config.getAppIcons()->executeIcon));
	setFixedSize(32, 32);
	setStyleSheet(
			"border: 0px;"
	);

	// set tooltip for the run buttons
	setToolTip(
			"Run code."
			" & "
			"Highlighted code."
	);

	CodeRunner::setupSignals();
}

void CodeRunner::runCode() {
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

void CodeRunner::setupWorkerThread() {
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
	connect(minion, &Minion::progressUpdated, this, &CodeRunner::handleProgress);
	connect(minion, &Minion::resultReady, this, &CodeRunner::handleTaskResults);
	connect(minion, &Minion::workFinished, this, &CodeRunner::handleWorkerFinished, Qt::QueuedConnection);

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

void CodeRunner::handleTaskResults(const QVariant &result) {
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

void CodeRunner::handleProgress(int i) {
	emit statusUpdate("Executing...");
}

void CodeRunner::handleWorkerFinished() {
	emit statusUpdate("Completed");

	workerThread = nullptr;
	minion = nullptr;
}

CodeRunner::~CodeRunner() {
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

void CodeRunner::setupSignals() {
	// Signal to execute the code
	connect(this, &IconButton::clicked, this, &CodeRunner::runCode);

	auto *appUi_ = dynamic_cast<AppUi *>(appUi);
	// Signal to update status bar in AppUI component for the running process
	connect(this, &CodeRunner::statusUpdate, appUi_, &AppUi::processStatusSlot);

	// Signal to update the out component in AppUI component for the completed process
	connect(this, &CodeRunner::updateOutputResult, appUi_, &AppUi::processResultSlot);
}
