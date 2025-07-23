//
// Created by talik on 5/31/2025.
//

#ifndef DIALOG_H
#define DIALOG_H


#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>

class QLabel;
class QPushButton;

class VersionUpdateDialog : public QDialog {
Q_OBJECT

public:
	explicit VersionUpdateDialog(QWidget *parent = nullptr);
	~VersionUpdateDialog() override = default;

	void setContent(const QString &messageLabel);

private:
	std::unique_ptr<QWidget> content;
	std::unique_ptr<QWidget> action;
	std::unique_ptr<QPushButton> updateButton;
	std::unique_ptr<QPushButton> cancelButton;
};

#endif //DIALOG_H
