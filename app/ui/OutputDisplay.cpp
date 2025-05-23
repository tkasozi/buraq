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
// Created by talik on 5/1/2024.
//
#include <QScrollArea>
#include <QDateTime>
#include "OutputDisplay.h"

#define OUTPUT_DISPLAY_STYLES R"(
	QWidget {
		background-color: #252525;
		border: 0px;
	}
)"

#define SCROLL_BAR_STYLES R"(
        QPlainTextEdit {
            /* Add padding so text doesn't touch the scrollbar directly if scrollbar is inside */
            padding-right: 5px;
            padding-bottom: 5px;
			font-family: Arial;
			font-size: 14px;
			color: #ffffff;
			height: 100vh;
			width: 100%;
			background-color: #232323;
        }

        /* Style for BOTH horizontal and vertical scrollbars */
        QScrollBar:vertical {
            background: #232323; /* Scrollbar track color */
            width: 15px;         /* Width of the vertical scr  ollbar */
            margin: 0px 0px 0px 0px; /* Top, Right, Bottom, Left margins */
        }
        QScrollBar::handle:vertical {
            background: #606060; /* Handle color */
            min-height: 20px;    /* Minimum height of the handle */
            border-radius: 7px;  /* Rounded corners for the handle */
        }
        QScrollBar::handle:vertical:hover {
            background: #707070; /* Handle color on hover */
        }
        QScrollBar::add-line:vertical { /* Up arrow button */
            background: #232323;
            height: 14px;
            subcontrol-position: bottom;
            subcontrol-origin: margin;
        }
        QScrollBar::sub-line:vertical { /* Down arrow button */
            background: #232323;
            height: 14px;
            subcontrol-position: top;
            subcontrol-origin: margin;
        }
        /* You can use an image for the arrows: */
        /*
        QScrollBar::up-arrow:vertical {
            image: url(:/icons/up_arrow.png);
        }
        QScrollBar::down-arrow:vertical {
            image: url(:/icons/down_arrow.png);
        }
        */
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none; /* Area between handle and arrows */
        }
    )"

OutputDisplay::OutputDisplay(QWidget *parent) : QWidget(parent) {
	setStyleSheet(OUTPUT_DISPLAY_STYLES);

	auto layout = new QGridLayout;
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	// TODO This will eventually become a tool bar
	auto pMainLabel = new QLabel;
	pMainLabel->setText("Output:");
	pMainLabel->setStyleSheet(
			"color: #fff;"
			"padding: 8px;"
			"border-bottom: 1px solid #000;"
	);
	layout->addWidget(pMainLabel, 0, 0, 1, 12);

	// For displaying the output
	main = std::make_unique<QPlainTextEdit>();

	layout->addWidget(main.get(), 1, 0, 11, 12);
	setLayout(layout);

	main->setReadOnly(true);
	main->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);

	main->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	if (parent != nullptr) {
		// main->setFixedHeight(100);
		main->setMinimumWidth(parent->width());
	}

	// --- QSS for Custom Scrollbar ---
	// Apply the stylesheet directly to the textEdit widget
	// This ensures only this textEdit's scrollbars are affected (and its children if any)
	main->setStyleSheet(SCROLL_BAR_STYLES);
	main->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

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
	QStringList errorsList = errorOutput.split("\\u2029");

	main->setFixedHeight(main->height() + std::max(((int) (list.size()* 1.3), (int) (errorsList.size()* 1.3)), 10));

	QLabel *error = createLabel(errorOutput, "error");
	auto layout = new QVBoxLayout;

	// Adds timestamp
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QString formattedDateTime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");  // Custom format
	main->appendHtml("<h4 style=\"color: #FFFDD0;\">Executed: " +
					 formattedDateTime + "</h4>");
	// End Adds timestamp

	main->appendHtml("<div style={\"background-color: inherit;\"}>");
	for (const QString &qString: list) {
		main->appendHtml("<p>" + qString + "</p>");
	}
	for (const QString &qString: errorsList) {
		// error logs
		main->appendHtml("<p style=\"color: #FF6347;\">" + qString + "</p>");
	}
	main->appendHtml("</div>");

	if (error != nullptr) layout->addWidget(error);
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
	recommendedSize.setHeight(
			recommendedSize.height() + std::max(100, recommendedSize.height())); // Add some extra width

	// Set the label's size manually
	label->setFixedSize(recommendedSize);

	return label;
}
