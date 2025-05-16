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
// Created by talik on 2/13/2024.
//

#ifndef IT_TOOLS_TOOLBAR_H
#define IT_TOOLS_TOOLBAR_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

#include "ToolButton.h"
#include "ToolBarEventFilter.h"
#include "../utils/Config.h"

class ToolBar : public QWidget {
Q_OBJECT

private:
	void configureLayout();

	// toolbar buttons and versionLabel
	ToolButton *maxRestoreBtn;
	ToolButton *minimizeBtn;
	ToolButton *closeBtn;
	QLabel *versionLabel;

private slots:

	void onCloseWindowButtonClicked();

	void onMinimizeWindowButtonClicked();

	void onMaximizeRestoreWindowButtonClicked();

public:
	explicit ToolBar(QWidget *parent = nullptr);
	void updateMaxAndRestoreIconButton();
	~ToolBar() override = default;
};


#endif //IT_TOOLS_TOOLBAR_H
