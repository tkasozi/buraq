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
// Created by talik on 2/8/2024.
//

#ifndef IT_TOOLS_TOOL_BUTTON_H
#define IT_TOOLS_TOOL_BUTTON_H

#include <QToolButton>
#include <QObject>
#include <QMouseEvent>
#include <QEvent>
#include <QStyleFactory>

#include "../utils/Utils.h"


class ToolButton : public QToolButton {
Q_OBJECT

private:
	QString customHoverColor;

	void applyIconStyles(const QIcon &icon) {
		if (!icon.isNull()) {
			ToolButton::setIcon(icon);
		}
		setFixedWidth(48);
		setFixedHeight(32);
		setMouseTracking(true);
	}

private slots:

public:
	explicit ToolButton(QWidget *ptr = nullptr) :  QToolButton(ptr) {};
	explicit ToolButton(const QString &fileName, QString customHoverColor = nullptr)
			: customHoverColor(std::move(customHoverColor)) {
		applyIconStyles(QIcon::fromTheme(fileName));
	};

	explicit ToolButton(const QIcon &icon, QString customHoverColor = nullptr)
			: customHoverColor(std::move(customHoverColor)) {
		applyIconStyles(icon);
	};

	void leaveEvent(QEvent *p) override {
		setStyleSheet(ItoolsNS::main_config.getMainStyles()->toolBar.styleSheet);
	}

	void enterEvent(QEnterEvent *event) override {
		if (customHoverColor != nullptr) {
			setStyleSheet(
					ItoolsNS::main_config.getMainStyles()->toolBar.styleSheet +
					"background-color: " + customHoverColor
			);
		} else {
			setStyleSheet(ItoolsNS::main_config.getMainStyles()->toolBarHover.styleSheet);
		}
	}

	~ToolButton() override = default;
};

#endif //IT_TOOLS_TOOL_BUTTON_H
