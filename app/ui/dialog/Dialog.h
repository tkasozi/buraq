//
// Created by talik on 5/31/2025.
//

#ifndef DIALOG_H
#define DIALOG_H


#include <QDialog>

class QLabel;
class QPushButton;

class Dialog : public QDialog {
Q_OBJECT

public:
	explicit Dialog(QWidget *parent = nullptr);
	~Dialog() override = default;

private slots:
	void onOkButtonClicked();

private:
	std::unique_ptr<QLabel> messageLabel;
	std::unique_ptr<QPushButton> okButton;
};

#endif //DIALOG_H
