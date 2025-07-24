//
// Created by talik on 6/7/2025.
//

#ifndef UPDATER_PROGRESS_DIALOG_H
#define UPDATER_PROGRESS_DIALOG_H


#include <QDialog>

// Forward declarations for UI elements to keep headers clean
class QLabel;
class QProgressBar;
class QPlainTextEdit;
class QPushButton;

class UpdateProgressDialog : public QDialog
{
Q_OBJECT

public:
	explicit UpdateProgressDialog(QWidget *parent = nullptr);
	~UpdateProgressDialog();

public slots:
	// Slots to receive updates from the worker thread
	void setStatusText(const QString& text);
	void addLogMessage(const QString& message);
	void setProgress(int percentage);
	void onUpdateFinished(bool success, const QString& finalMessage);

private:
	void setupUi();

	QLabel* m_statusLabel;
	QProgressBar* m_progressBar;
	QPlainTextEdit* m_logArea;
	QPushButton* m_finishButton;
};


#endif //UPDATER_PROGRESS_DIALOG_H
