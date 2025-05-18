//
// Created by talik on 5/1/2024.
//
#include <QScrollArea>
#include <QDateTime>
#include "OutputDisplay.h"

OutputDisplay::OutputDisplay(QWidget *parent) : QWidget(parent) {
	auto layout = new QGridLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	// TODO This will eventually become a tool bar
	auto pMainLabel = new QLabel;
	pMainLabel->setText("Output:");
	pMainLabel->setStyleSheet("padding: 8px;  border-bottom: 2px solid #232323;");
	layout->addWidget(pMainLabel, 0, 0, 1, 12);

	// For displaying the output
	main = new QWidget;
	if (parent != nullptr) {
		main->setFixedHeight(100);
		main->setFixedWidth(parent->width());
	}

	qResultLogsLayout = new QVBoxLayout;
	main->setLayout(qResultLogsLayout);

	scrollArea = new QScrollArea;
	scrollArea->setWidget(main);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	layout->addWidget(scrollArea, 1, 0, 11, 12);

	// Setting the scrollable area
	setLayout(layout);
	setStyleSheet("background-color: #252525; border: 0px;");
	hide();
}

void OutputDisplay::toggle() {
	if (isVisible()) {
		hide();
	} else {
		show();
	}
}

void OutputDisplay::log(const QString &strOutput, const QString &errorOutput) {
	QStringList list = strOutput.split("\\u2029");

	QLabel *error = createLabel(errorOutput, "error");

	// This new log to show/print
	auto thisLog = new QWidget;
	//@formatter:off
	thisLog->setStyleSheet(
			"border-radius: 10px;"
			"margin: 8px;"
			"padding: 8px;"
			"background-color: #232323;"
	);
	//@formatter:on

	auto layout = new QVBoxLayout;
	thisLog->setLayout(layout);

	// Adds timestamp
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");  // Custom format
	auto exeTime = new QLabel;
	exeTime->setFixedWidth(250);
	exeTime->setText("Executed: " + formattedDateTime);
	exeTime->setStyleSheet("color: #FFFDD0;");
	exeTime->setFont(QFont("Arial", 32));
	layout->addWidget(exeTime);
	// End Adds timestamp

	for (const QString &qString: list) {
		QLabel *output = createLabel(qString);

		int height = 0;
		if (output != nullptr) { // Add to the total height of this log
			QSize recommendedSize = output->sizeHint();
			height += recommendedSize.height();
		}

		if (error != nullptr) { // Add to the total height of this log
			QSize recommendedSize = error->sizeHint();
			height += recommendedSize.height();
		}

		main->setFixedHeight(main->height() + std::max((int) (height * 1.3), 10));

		// Added after adjusting the main content area height in order to get the right height
		if (output != nullptr) layout->addWidget(output);
	}

	if (error != nullptr) layout->addWidget(error);
	qResultLogsLayout->addWidget(thisLog);
}

QLabel *OutputDisplay::createLabel(const QString &text, QString state) {
	if (text.isEmpty()) return nullptr;

	auto label = new QLabel;
	label->setText(text);
	label->setFont(QFont("Arial", 32));

	//@formatter:off
	label->setStyleSheet(
			std::equal(state.begin(), state.end(), "error") ?
			"color: #FF6347;":
			"color: #F9F6EE;");
	//@formatter:on

	label->setAlignment(Qt::AlignLeft);

	// Call adjustSize() after setting the text
	label->adjustSize();

	// Get the recommended size from sizeHint()
	QSize recommendedSize = label->sizeHint();
	recommendedSize.setWidth(recommendedSize.width() * 2); // Add some extra width
	recommendedSize.setHeight(recommendedSize.height() +  std::max(100, recommendedSize.height()));
	// Set the label's size manually
	label->setFixedSize(recommendedSize);

	return label;
}
