//
// Created by Talik Kasozi on 2/3/2024.
//

#include "IToolsUi.h"
#include "ToolBar.h"
#include "IconButton.h"
#include "Editor.h"
#include "CustomDrawer.h"
#include "EditorMargin.h"
#include "../utils/Utils.h"
#include <sstream>

// TODO -- rename variable to be more readable and add documentation
IToolsUi::IToolsUi(QWidget *parent) : QMainWindow(parent) {
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	// This can be updated dynamically.
	QIcon::setThemeName("dark");

	// overall bgColor
	setStyleSheet("background-color: #232323; color: #606060;");

	// setting up default window size
	const WindowConfig& windowConfig = ItoolsNS::main_config.getWindow();
	resize(windowConfig.normalSize, windowConfig.minHeight);
	setMinimumSize(windowConfig.minWidth, windowConfig.minHeight);

	// Add Tool bar
	toolBar = new ToolBar(this);
	setMenuWidget(toolBar);

	// Add Status bar
	statusBar = new QStatusBar;
	statusBar->setStyleSheet(IToolsNamespace::Styles::statusToolBar);
	setStatusBar(statusBar);

	// Setting window title and docking icon
	setWindowTitle(ItoolsNS::main_config.getTitle());
	setWindowIcon(ItoolsNS::main_config.getAppLogo());

	// Add CentralWidget
	auto *centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	// Add layout to centralWidget
	centralWidgetLayout = new QGridLayout;
	centralWidgetLayout->setSpacing(0);
	centralWidgetLayout->setContentsMargins(0, 0, 0, 0);
	centralWidget->setLayout(centralWidgetLayout);

	// Add control panel to the central widget.
	auto *centralWidgetLayout2 = new QGridLayout;
	centralWidgetLayout2->setSpacing(0);
	centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);

	auto *centralWidgetControlPanel = new QWidget;
	centralWidgetControlPanel->setStyleSheet(IToolsNamespace::Styles::centralControlToolBar);
	centralWidgetControlPanel->setFixedWidth(64);
	centralWidgetControlPanel->setLayout(centralWidgetLayout2);

	// panel A
	auto *layoutA = new QVBoxLayout;
	layoutA->setSpacing(1);
	layoutA->setContentsMargins(0, 0, 0, 0);

	auto *centralWidgetControlPanelA = new QWidget;
	centralWidgetControlPanelA->setStyleSheet("border: none;");
	centralWidgetControlPanelA->setLayout(layoutA);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignJustify);

	// space between
//	centralWidgetLayout2->addWidget(new QWidget, 1, 0, 11, 12);

	folderButton = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons().folderIcon));

	layoutA->addWidget(folderButton);

	// Good, to be placed in panel B
	auto *layoutB = new QVBoxLayout;
	layoutB->setSpacing(8);

	auto *centralWidgetControlPanelB = new QWidget;
	centralWidgetControlPanelB->setLayout(layoutB);
	centralWidgetControlPanelB->setContentsMargins(16, 0, 0, 8);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 12, 12, Qt::AlignBottom);

	auto outputButton = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons().terminalIcon));
	connect(outputButton, &IconButton::clicked, this, &IToolsUi::onShowOutputButtonClicked);
	layoutB->addWidget(outputButton);

	auto *settingButton = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons().settingsIcon));
	layoutB->addWidget(settingButton);

	// add to central widget
	centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);

	// Component
//	centralWidgetLayout->setColumnMinimumWidth(1, 0);
	connect(folderButton, &IconButton::clicked, this, &IToolsUi::onClicked);

	// layout c
	layoutB->setSpacing(0);
	layoutB->setContentsMargins(0, 0, 0, 0);


	// This where the output generated after executing the script will be displayed
	outPutArea = new OutputDisplay(this);

	// The text or script editor.
	editorMargin = new EditorMargin(this);
	itoolsEditor = new Editor(this);

	drawer = new CustomDrawer(itoolsEditor, this);

	placeHolderLayout = new QGridLayout;
	placeHolderLayout->setSpacing(0);
	placeHolderLayout->setContentsMargins(0, 0, 0, 0);

	placeHolderLayout->addWidget(drawer, 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
	placeHolderLayout->addWidget(editorMargin, 0, 2, Qt::AlignmentFlag::AlignTop);
	placeHolderLayout->addWidget(itoolsEditor, 0, 3, 12, 10);

	// add main content area
	editorAndDrawerAreaPanel = new QWidget;
	editorAndDrawerAreaPanel->setLayout(placeHolderLayout);
	centralWidgetLayout->addWidget(editorAndDrawerAreaPanel, 0, 2, 12, 12);
	centralWidgetLayout->addWidget(outPutArea, 6, 2, 6, 12);

	// Add a temporary message that disappears after 5 seconds
	statusBar->showMessage("Ready.", 2000);
}

void IToolsUi::onClicked() {
	drawer->toggle();
	if (drawer->isVisible()) {
		placeHolderLayout->addWidget(drawer, 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
	} else {
		placeHolderLayout->removeWidget(drawer);
	}
}

void IToolsUi::onShowOutputButtonClicked() {
	outPutArea->toggle();
	if (outPutArea->isVisible()) {
		centralWidgetLayout->addWidget(outPutArea, 6, 2, 6, 12);
	} else {
		centralWidgetLayout->removeWidget(outPutArea);
	}
}

OutputDisplay *IToolsUi::getOutputDisplay() {
	return outPutArea;
}

Editor *IToolsUi::getEditor() {
	return itoolsEditor;
}

QStatusBar *IToolsUi::getQStatusBar() {
	return statusBar;
}
