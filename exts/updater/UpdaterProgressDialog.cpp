//
// Created by talik on 6/7/2025.
//

#include "UpdaterProgressDialog.h"

#include <QLabel>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication> // For QApplication::processEvents()

UpdateProgressDialog::UpdateProgressDialog(QWidget *parent)
		: QDialog(parent)
{
	setupUi();
}

void UpdateProgressDialog::setupUi()
{
	setWindowTitle("Updating Application...");
	setFixedSize(450, 300); // A fixed size often looks good for progress dialogs

	// Prevent closing via the 'X' button while the update is in progress
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint);

	m_statusLabel = new QLabel("Initializing update...", this);
	m_statusLabel->setStyleSheet("font-weight: bold;");

	m_progressBar = new QProgressBar(this);
	m_progressBar->setRange(0, 100);
	m_progressBar->setValue(0);
	m_progressBar->setTextVisible(true);

	m_logArea = new QPlainTextEdit(this);
	m_logArea->setReadOnly(true);

	m_finishButton = new QPushButton("Close", this);
	m_finishButton->setVisible(false); // Hide until update is complete
	connect(m_finishButton, &QPushButton::clicked, this, &UpdateProgressDialog::accept); // accept() closes the dialog

	auto *mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_statusLabel);
	mainLayout->addWidget(m_progressBar);
	mainLayout->addWidget(m_logArea, 1); // The '1' makes the log area stretch
	mainLayout->addWidget(m_finishButton);

	setLayout(mainLayout);
}

void UpdateProgressDialog::setStatusText(const QString& text) const
{
	m_statusLabel->setText(text);
}

void UpdateProgressDialog::addLogMessage(const QString& message) const
{
	m_logArea->appendPlainText(message);
	QApplication::processEvents(); // Process events to ensure the UI updates immediately
}

void UpdateProgressDialog::setProgress(const int percentage) const
{
	m_progressBar->setValue(percentage);
}

void UpdateProgressDialog::onUpdateFinished(bool success, const QString& finalMessage)
{
	setWindowTitle(success ? "Update Complete" : "Update Failed");
	setStatusText(finalMessage);

	if (success) {
		m_progressBar->setValue(100);
		// m_progressBar->setStyleSheet("QProgressBar::chunk { background-color: #4CAF50; }"); // Green for success
	} else {
		// m_progressBar->setStyleSheet("QProgressBar::chunk { background-color: #F44336; }"); // Red for failure
		m_finishButton->setVisible(true); // Show the "Finish" button
	}

}