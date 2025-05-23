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
// Created by talik on 2/16/2024.
//

#ifndef IT_TOOLS_ICON_BUTTON_H
#define IT_TOOLS_ICON_BUTTON_H

#include <QString>
#include <QWidget>

#include "ToolButton.h"

class IconButton : public ToolButton {

public:
	explicit IconButton(const QString &fileName, const int w = 32, int h = 32, const QString &customStyles = "")
			: ToolButton(fileName) {
		setFixedSize(w, h);
		setStyleSheet(customStyles + "border: 0px;");
		this->customStyles = customStyles;
	}

	explicit IconButton(const QIcon &icon, const int w = 32, int h = 32, const QString &customStyles = "")
			: ToolButton(icon) {
		setFixedSize(w, h);
		setStyleSheet(customStyles + "border: 0px;");
		this->customStyles = customStyles;
	}

	void leaveEvent(QEvent *p) override {
		setStyleSheet(
				"border: 0px;" +
				ItoolsNS::main_config.getMainStyles()->commonStyle.styleSheet
		);
	}

	void enterEvent(QEnterEvent *event) override {
		if (customStyles != "") {
			setStyleSheet(
					"border-radius: 10px;" +
					customStyles
			);
		} else {
			setStyleSheet(
					"border-radius: 10px;" +
					ItoolsNS::main_config.getMainStyles()->toolBarHover.styleSheet
			);
		}
	}

	~IconButton() override = default;

private:
	QString customStyles;
};

#endif //IT_TOOLS_ICON_BUTTON_H
