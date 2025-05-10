//
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
	setStyleSheet(IToolsNamespace::Styles::toolBarBackgroundColor);

	// initializing control buttons.
	this->versionLabel = new QLabel(IToolsNamespace::appVersion);
	this->minimizeBtn = new ToolButton(IToolsNamespace::AppIcons::minimize);
	this->closeBtn = new ToolButton(IToolsNamespace::AppIcons::close, "red");
	// by default, window is minimized.
	this->maxRestoreBtn = new ToolButton(IToolsNamespace::AppIcons::maximize);

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
	QIcon maximizeIcon(QIcon::fromTheme(IToolsNamespace::AppIcons::maximize));
	QIcon restoreIcon(QIcon::fromTheme(IToolsNamespace::AppIcons::restore));

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
