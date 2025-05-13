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

// Created by talik on 2/13/2024.
//

#include "ToolBar.h"

ToolBar::ToolBar(QWidget *parent) {
	// This is the component on which the toolbar is installed.
	ToolBar::setParent(parent);

	// Adds filter to handle dragging the window, and double-clicking to max/min the window, etc.
	auto *filter = new ToolBarEventFilter();
	installEventFilter(filter);

	// Toolbar css
	setStyleSheet(ItoolsNS::main_config.getMainStyles().toolBar.styleSheet);

	// initializing control buttons.
	this->versionLabel = new QLabel(ItoolsNS::main_config.getVersion());
	this->minimizeBtn = new ToolButton(QIcon(ItoolsNS::main_config.getWindow().minimizeIcon));
	this->closeBtn = new ToolButton(QIcon(ItoolsNS::main_config.getWindow().closeIcon), "red");
	// by default, window is minimized.
	this->maxRestoreBtn = new ToolButton(QIcon(ItoolsNS::main_config.getWindow().maximizeIcon));

	// sets up the grid layout on this component
	configureLayout();

	// Connect the "isActive" signal of the button to the "onButtonClicked" slot
	connect(minimizeBtn, &ToolButton::clicked, this, &ToolBar::onMinimizeWindowButtonClicked);
	connect(maxRestoreBtn, &ToolButton::clicked, this, &ToolBar::onMaximizeRestoreWindowButtonClicked);
	connect(closeBtn, &ToolButton::clicked, this, &ToolBar::onCloseWindowButtonClicked);
}

void ToolBar::onCloseWindowButtonClicked() {
	// closing the window.
	auto *parent = dynamic_cast<QMainWindow *>(this->parent());
	parent->close();
}

void ToolBar::onMinimizeWindowButtonClicked() {
	auto *parent = dynamic_cast<QMainWindow *>(this->parent());
	parent->showMinimized();
}

void ToolBar::onMaximizeRestoreWindowButtonClicked() {
	updateMaxAndRestoreIconButton();
}

void ToolBar::updateMaxAndRestoreIconButton() {
	QIcon maximizeIcon(ItoolsNS::main_config.getWindow().maximizeIcon);
	QIcon restoreIcon(ItoolsNS::main_config.getWindow().restoreIcon);

	auto *parent = dynamic_cast<QMainWindow *>(this->parent());
	if (parent != nullptr) {

		const bool isMax = parent->isMaximized();
		// restore, maximize
		if (!isMax) {
			maxRestoreBtn->setIcon(restoreIcon);
			parent->showMaximized();
		} else {
			maxRestoreBtn->setIcon(maximizeIcon);
			parent->showNormal();
		}
	}
}

void ToolBar::configureLayout() {
	auto *toolBarGrid = new QGridLayout();
	this->setLayout(toolBarGrid);

	// No extra spacing both inside and out
	toolBarGrid->setSpacing(0);
	toolBarGrid->setContentsMargins(0, 0, 0, 0);

	// formatting the version label
	versionLabel->setAlignment(Qt::AlignVCenter);

	// Add the window controls to the toolbar
	auto *emptySpace = new QWidget();
	toolBarGrid->addWidget(emptySpace, 0, 0, 1, 5);
	toolBarGrid->addWidget(versionLabel, 0, 5, 1, 1); // Span three columns

	auto *emptySpace2 = new QWidget();
	toolBarGrid->addWidget(emptySpace2, 0, 6, 1, 3);
	toolBarGrid->addWidget(minimizeBtn, 0, 9, 1, 1);  // Span three columns
	toolBarGrid->addWidget(maxRestoreBtn, 0, 10, 1, 1);  // Span three columns
	toolBarGrid->addWidget(closeBtn, 0, 11, 1, 1);  // Span three columns
}
