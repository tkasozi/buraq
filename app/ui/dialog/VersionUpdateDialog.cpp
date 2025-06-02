//
// Created by talik on 5/31/2025.
//

#include "VersionUpdateDialog.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout> // For layout

VersionUpdateDialog::VersionUpdateDialog(QWidget *parent)
		: QDialog(parent) {

	setFixedWidth(400);
	setMinimumHeight(100);

	content = std::make_unique<QWidget>(this);

	action = std::make_unique<QWidget>(this);
	auto action_layout = new QHBoxLayout(action.get());

	updateButton = std::make_unique<QPushButton>("Update", action.get());
	updateButton->setObjectName("updateButton"); // Set the object name

	cancelButton = std::make_unique<QPushButton>("Later", action.get());
	cancelButton->setObjectName("laterButton");   // Set the object name

	action_layout->addWidget(updateButton.get());
	action_layout->addWidget(cancelButton.get());

	auto layout = new QVBoxLayout(this);
	layout->addWidget(content.get());
	layout->addWidget(action.get());
	setLayout(layout);

	// Connect the button's clicked signal to our slot or directly to QDialog's accept()
	connect(updateButton.get(), &QPushButton::clicked, this, &VersionUpdateDialog::accept);
	connect(cancelButton.get(), &QPushButton::clicked, this, &VersionUpdateDialog::close);
}

void VersionUpdateDialog::setContent(const QString &notesLabel) {
	QStringList notesList = notesLabel.split(",");
	auto contentLayout = new QVBoxLayout(content.get());
	int i = 0;
	for (QString &note: notesList) {
		auto label = new QLabel(QString::number(++i) + ". " + note, content.get());
		contentLayout->addWidget(label);
	}
}
