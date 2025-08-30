//
// Created by talik on 5/29/2025.
//

#include "FramelessWindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel> // For an example title

#include "app_version.h"
#include "Config.h"
#include "ToolBarEventFilter.h"
#include "CustomDrawer.h"
#include "IconButton.h"
#include "output_display/OutputDisplay.h"
#include "ToolBar.h"
#include "PluginManager.h"
#include "EditorMargin.h"
#include "app_ui/AppUi.h"

#include "ToolBarEventFilter.h" // events?


FramelessWindow::FramelessWindow(QWidget *parent)
	: QWidget(parent), m_statusBar(new QStatusBar(this)), m_dragging(false)
{

	// setting up default m_window size
	const auto windowConfig = Config::singleton().getWindow();
	this->setMinimumSize(windowConfig->normalSize, windowConfig->minHeight);

	// Set the m_window flag to remove the default frame
	this->setWindowFlags(Qt::FramelessWindowHint);
	// allow the m_window to be transparent if you have rounded corners.
	this->setAttribute(Qt::WA_TranslucentBackground);

	// Main vertical layout for the whole m_window
	const auto mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0); // No margins for the main layout
	mainLayout->setSpacing(0);

	// Custom Title Bar
	m_titleBar = new QWidget(this);
	m_titleBar->setFixedHeight(35); // Set your desired title bar height
	m_titleBar->setObjectName("customTitleBar"); // For styling

	const auto titleBarLayout = new QHBoxLayout(m_titleBar);
	titleBarLayout->setContentsMargins(0, 0, 0, 0);
	titleBarLayout->setSpacing(0);

	const auto titleText = new QLabel(m_titleBar);
	titleText->setObjectName("titleText");

	m_minimizeButton = new QPushButton("—", m_titleBar); // Underscore for minimize
	m_maximizeButton = new QPushButton("☐", m_titleBar); // Square for maximize/restore
	m_closeButton = new QPushButton("✕", m_titleBar);    // X for close
	m_closeButton->setObjectName("closeButton"); // For specific styling

	m_minimizeButton->setFixedSize(40, m_titleBar->height());
	m_maximizeButton->setFixedSize(40, m_titleBar->height());
	m_closeButton->setFixedSize(40, m_titleBar->height());

	titleBarLayout->addWidget(titleText);
	titleBarLayout->addStretch();
	titleBarLayout->addWidget(m_minimizeButton);
	titleBarLayout->addWidget(m_maximizeButton);
	titleBarLayout->addWidget(m_closeButton);

	mainLayout->addWidget(m_titleBar);

	// Add Tool bar
	m_toolBar = std::make_unique<ToolBar>(this); // constructor init list
	// Add the File menu first
	m_toolBar->addFileMenu();
	mainLayout->addWidget(m_toolBar.get());

	// Content Area (example)
	const auto contentArea = new QWidget(this);
	initContentAreaLayout(contentArea);
	mainLayout->addWidget(contentArea, 1); // Content area takes remaining space

	// Status bar
	m_statusBar->setObjectName("appStatusBar");
	m_statusBar->setSizeGripEnabled(false);
	mainLayout->addWidget(m_statusBar);

	setLayout(mainLayout);

	// Connections
	connect(m_minimizeButton, &QPushButton::clicked, this, &FramelessWindow::handleMinimize);
	connect(m_maximizeButton, &QPushButton::clicked, this, &FramelessWindow::handleMaximizeRestore);
	connect(m_closeButton, &QPushButton::clicked, this, &FramelessWindow::handleClose);
}

FramelessWindow::~FramelessWindow() = default;

void FramelessWindow::mousePressEvent(QMouseEvent *event) {
	// Check if the press is on the title bar area (but not on the buttons)
	if (event->button() == Qt::LeftButton && m_titleBar->geometry().contains(event->pos())) {
		// Check if the click was on any of the buttons
		bool onButton = false;
		for (const QPushButton* btn : {m_minimizeButton, m_maximizeButton, m_closeButton}) {
			if (btn->geometry().contains(m_titleBar->mapFromParent(event->pos()))) {
				onButton = true;
				break;
			}
		}
		if (!onButton) {
			m_dragging = true;
			m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
			event->accept();
			return;
		}
	}
	QWidget::mousePressEvent(event); // Pass on to other handlers or base class
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event) {
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		move(event->globalPosition().toPoint() - m_dragPosition);
		event->accept();
	} else {
		QWidget::mouseMoveEvent(event);
	}
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_dragging = false;
	}
	QWidget::mouseReleaseEvent(event);
}

void FramelessWindow::handleMinimize() {
	this->showMinimized();
}

void FramelessWindow::handleMaximizeRestore() {
	if (this->isMaximized()) {
		this->showNormal();
		m_maximizeButton->setText("☐"); // Restore symbol
	} else {
		this->showMaximized();
		m_maximizeButton->setText("❐"); // Actual maximize symbol (might need specific font or icon)
	}
}

void FramelessWindow::handleClose() {
	this->close();
}

void  FramelessWindow::initContentAreaLayout(QWidget* contentArea)
{
	// Add layout to centralWidget
	m_centralWidgetLayout = std::make_unique<QGridLayout>(contentArea);
	m_centralWidgetLayout->setSpacing(0);
	m_centralWidgetLayout->setContentsMargins(0, 0, 0, 0);

	// Setting m_window title and docking icon
	setWindowTitle(UpdateInfo().currentVersion.data());
	setWindowIcon(Config::singleton().getAppLogo());

	// Add control panel to the central widget.
	const auto centralWidgetLayout2 = new QGridLayout(this);
	centralWidgetLayout2->setSpacing(0);
	centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);

	const auto centralWidgetControlPanel = new QWidget(this);
	centralWidgetControlPanel->setObjectName("ControlToolBar");

	centralWidgetControlPanel->setFixedWidth(64);
	centralWidgetControlPanel->setLayout(centralWidgetLayout2);

	// panel A
	auto* layoutA = new QVBoxLayout(this);
	layoutA->setSpacing(1);
	layoutA->setContentsMargins(0, 0, 0, 0);

	auto* centralWidgetControlPanelA = new QWidget(this);
	centralWidgetControlPanelA->setLayout(layoutA);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignJustify);

	const auto appIcons = Config::singleton().getAppIcons();
	m_folderButton = std::make_unique<IconButton>(appIcons->folderIcon);

	layoutA->addWidget(m_folderButton.get());

	// Good, to be placed in panel B
	auto* layoutB = new QVBoxLayout(this);
	layoutB->setSpacing(8);

	auto* centralWidgetControlPanelB = new QWidget(this);
	centralWidgetControlPanelB->setLayout(layoutB);
	centralWidgetControlPanelB->setContentsMargins(16, 0, 0, 8);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 12, 12, Qt::AlignBottom);

	const auto outputButton = new IconButton(appIcons->terminalIcon);
	connect(outputButton, &IconButton::clicked, this, &FramelessWindow::onShowOutputButtonClicked);
	layoutB->addWidget(outputButton);

	const auto settingsButton = new IconButton(appIcons->settingsIcon);
	layoutB->addWidget(settingsButton);

	// add to central widget
	m_centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);

	// Component
	connect(m_folderButton.get(), &IconButton::clicked, this, &FramelessWindow::updateDrawer);

	// layout c
	layoutB->setSpacing(0);
	layoutB->setContentsMargins(0, 0, 0, 0);

	// Editor helper component.
	m_itoolsEditor = std::make_unique<Editor>(this);

	// Handles file nav
	m_drawer = std::make_unique<CustomDrawer>(m_itoolsEditor.get());
	// This where the output_display generated after executing the script will be displayed
	m_outPutArea = std::make_unique<OutputDisplay>(this);

	m_placeHolderLayout = std::make_unique<QGridLayout>();
	m_placeHolderLayout->setSpacing(0);
	m_placeHolderLayout->setContentsMargins(0, 0, 0, 0);

	m_placeHolderLayout->addWidget(m_drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
	m_placeHolderLayout->addWidget(m_itoolsEditor.get(), 0, 3, 12, 1);

	// add main content area
	const auto editorAndDrawerAreaPanel = new QWidget(this);
	editorAndDrawerAreaPanel->setLayout(m_placeHolderLayout.get());
	m_centralWidgetLayout->addWidget(editorAndDrawerAreaPanel, 0, 2, 12, 12);
	m_centralWidgetLayout->addWidget(m_outPutArea.get(), 6, 2, 6, 12);
}


void FramelessWindow::processStatusSlot(const QString& message, const int timeout) const
{
	m_statusBar->showMessage(message, timeout);
}

void FramelessWindow::processResultSlot(const int exitCode, const QString& output, const QString& error) const
{
	m_outPutArea->show();

	if (exitCode == 0)
	{
	    m_outPutArea->log(output, error);

	    processStatusSlot(error.isEmpty() ? "Completed!" : "Completed with errors.");
	}
	else
	{
	    processStatusSlot("Process failed!");
	    m_outPutArea->log("", error);
	}
}

void FramelessWindow::updateDrawer() const
{
	m_drawer->toggle();
	if (m_drawer->isVisible())
	{
	    m_placeHolderLayout->addWidget(m_drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
	}
	else
	{
	    m_placeHolderLayout->removeWidget(m_drawer.get());
	}
}

void FramelessWindow::onShowOutputButtonClicked() const
{
	m_outPutArea->toggle();
	if (m_outPutArea->isVisible())
	{
	    m_centralWidgetLayout->addWidget(m_outPutArea.get(), 6, 2, 6, 12);
	}
	else
	{
	    m_centralWidgetLayout->removeWidget(m_outPutArea.get());
	}
}

EditorMargin* FramelessWindow::getEditorMargin() const
{
	return editorMargin.get();
}

Editor* FramelessWindow::getEditor() const
{
	return m_itoolsEditor.get();
}

PluginManager* FramelessWindow::getLangPluginManager() const
{
	return pluginManager.get();
}
