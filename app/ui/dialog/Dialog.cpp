//
// Created by talik on 5/31/2025.
//

#include "Dialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout> // For layout

Dialog::Dialog(QWidget *parent)
		: QDialog(parent) {
	setWindowTitle("My Custom Dialog");

	messageLabel = std::make_unique<QLabel>("This is a custom dialog!", this);
	okButton = std::make_unique<QPushButton>("OK", this);

	auto *layout = new QVBoxLayout(this);
	layout->addWidget(messageLabel.get());
	layout->addWidget(okButton.get());
	setLayout(layout);

	// Connect the button's clicked signal to our slot or directly to QDialog's accept()
	connect(okButton.get(), &QPushButton::clicked, this, &Dialog::accept);
	// QDialog::accept() will close the dialog and make exec() return QDialog::Accepted
}

// Optional custom slot if you need to do more before accepting
void Dialog::onOkButtonClicked() {
	// Do something before closing, if needed
	qDebug("Custom OK button clicked, now accepting.");
	accept(); // This closes the dialog and signals Accepted
}