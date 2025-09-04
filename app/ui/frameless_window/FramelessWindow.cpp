//
// Created by talik on 5/29/2025.
//

#include "FramelessWindow.h"

#include <QStatusBar>
#include <QVBoxLayout>
#include <QSplitter>

#include "Config.h"
#include "../Filters/Toolbar/ToolBarEvent.h"
#include "CustomDrawer.h"
#include "IconButton.h"
#include "output_display/OutputDisplay.h"
#include "ToolBar.h"
#include "PluginManager.h"
#include "app_ui/AppUi.h"
#include "Filters/ThemeManager/ThemeManager.h"
#include "../settings/Dialog/SettingsDialog.h"
#include "settings/SettingManager/SettingsManager.h"

#include <QIcon>

#include "Frame/Frame.h"

// Install themes, custom events, window size
void FramelessWindow::init()
{
    // Initialize the ThemeManager instance
    this->installEventFilter(&themeManager);

    // Set the m_window flag to remove the default frame
    this->setWindowFlags(Qt::FramelessWindowHint);
    // allow the m_window to be transparent if you have rounded corners.
    this->setAttribute(Qt::WA_TranslucentBackground);

    // setting up default m_window size
    this->resize(userPreferences.windowSize);
    this->move(userPreferences.windowPosition);

    // Generate the icon at runtime
    const auto appLogo = QIcon(":/icons/buraq.ico");

    // Set it as the window icon
    this->setWindowIcon(appLogo);
}

FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent),
      themeManager(ThemeManager::instance()),
      m_Frame(std::make_unique<Frame>(this)),
      userPreferences(settingsManager.loadSettings()),
      m_dragPosition(QPoint(0, 0))
{
    init();

    // Set the central widget for the QMainWindow
    this->setCentralWidget(m_Frame.get());

    // Custom Title Bar
    const auto m_titleBar_ = m_Frame->getTitleBar();
    // const auto m_titlebarEvents = new ToolBarEvent(m_titleBar_);
    // m_titleBar_->installEventFilter(m_titlebarEvents);

    m_minimizeButton = std::make_unique<QPushButton>("—", m_titleBar_); // Underscore for minimize
    m_minimizeButton->setObjectName("minimizeButton"); // For specific styling

    m_maximizeButton = std::make_unique<QPushButton>("☐", m_titleBar_); // Square for maximize/restore
    m_maximizeButton->setObjectName("maximizeButton"); // For specific styling

    m_settingsButton = std::make_unique<QPushButton>("⋮", m_titleBar_);
    m_settingsButton->setToolTip("IDE and Project Settings");
    m_settingsButton->setObjectName("settingGearButton");

    m_settingsButton->setFixedSize(40, m_titleBar_->height());
    m_minimizeButton->setFixedSize(40, m_titleBar_->height());
    m_maximizeButton->setFixedSize(40, m_titleBar_->height());

    const auto titleBarLayout = m_Frame->getExtraButtonsLayout();
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(m_settingsButton.get());
    titleBarLayout->addWidget(m_minimizeButton.get());
    titleBarLayout->addWidget(m_maximizeButton.get());

    // Add Tool bar
    const auto toolkitBar = m_Frame->getToolKitBar();
    m_toolBar = std::make_unique<ToolBar>(toolkitBar);
    m_toolBar->setFixedHeight(35);
    m_toolBar->addFileMenu(); // Add the File menu first
    if (const auto layout = toolkitBar->layout(); layout)
    {
        layout->addWidget(m_toolBar.get());
    }

    // Content Area (example)
    initContentAreaLayout(m_Frame->getMainContentWidget());

    // Status bar
    const auto bottomPanel = m_Frame->getBottomPanelWidget();
    m_statusBar = new QStatusBar(bottomPanel);
    m_statusBar->setObjectName("appStatusBar");
    m_statusBar->setFixedHeight(25);
    bottomPanel->layout()->addWidget(m_statusBar);

    // Connections
    connect(m_maximizeButton.get(), &QPushButton::clicked, this, &FramelessWindow::showMaximizeOrRestoreSlot);
    connect(m_minimizeButton.get(), &QPushButton::clicked, this, &FramelessWindow::showMinimized);
    connect(this, &FramelessWindow::closeApp, this, &FramelessWindow::close);

    // Create an instance of your settings dialog
    const auto settingsDialog = new SettingsDialog(this);
    // Connect the button's click signal to open the dialog
    connect(m_settingsButton.get(), &QPushButton::clicked, settingsDialog, &SettingsDialog::exec);
}

// smart pointers will be cleaned up by std::unique_ptr
FramelessWindow::~FramelessWindow()
{
    // save the last window size & position
    userPreferences.windowSize = this->size();
    userPreferences.windowPosition = m_dragPosition;
    settingsManager.saveSettings(userPreferences);
};

void FramelessWindow::closeWindowSlot()
{
    emit closeApp();
}

void FramelessWindow::showMaximizeOrRestoreSlot()
{
    if (this->isMaximized())
    {
        this->showNormal();
        m_maximizeButton->setText("☐"); // Restore symbol
    }
    else
    {
        this->showMaximized();
        m_maximizeButton->setText("❐"); // Actual maximize symbol (might need specific font or icon)
    }
}

void FramelessWindow::initContentAreaLayout(QWidget* mainContentArea)
{
    // Editor helper component.
    m_editor = std::make_unique<Editor>(this);
    // Handles file nav
    m_drawer = std::make_unique<CustomDrawer>(m_editor.get());
    // This where the output_display generated after executing the script will be displayed
    m_outPutArea = std::make_unique<OutputDisplay>(this);

    const auto contentArea = new QWidget(mainContentArea);
    mainContentArea->layout()->addWidget(contentArea);

    // 1. MAIN LAYOUT: A horizontal layout to separate the left controls from the main content.
    QHBoxLayout* mainLayout = new QHBoxLayout(contentArea);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 2. LEFT CONTROL PANEL: A vertical layout for the top and bottom buttons.
    const auto leftPanelLayout = m_Frame->getLeftSidePanelLayout();

    QPushButton* folderButton = new QPushButton("🗀", leftPanelLayout->widget());
    folderButton->setFixedSize(35, 35);

    QPushButton* outputButton = new QPushButton("❯_", leftPanelLayout->widget());
    outputButton->setFixedSize(35, 35);

    leftPanelLayout->addWidget(folderButton); // Add button to the top
    leftPanelLayout->addStretch(); // Pushes buttons to top and bottom
    leftPanelLayout->addWidget(outputButton); // Add button to the bottom

    // 3a. Top Area (Editor + Drawer)
    QWidget* topAreaWidget = new QWidget();
    QHBoxLayout* topAreaLayout = new QHBoxLayout(topAreaWidget);
    topAreaLayout->setSpacing(0);
    topAreaLayout->setContentsMargins(0, 0, 0, 0);

    // 3. MAIN VERTICAL SPLITTER: Separates the top (editor/drawer) from the bottom (output).
    QSplitter* rightSideSplitter = new QSplitter(Qt::Vertical);

    // 4. HORIZONTAL SPLITTER: This will go in the top section of the vertical splitter.
    // It separates the drawer from the editor.
    auto* topAreaSplitter = new QSplitter(Qt::Horizontal);

    // Add the drawer and editor to the HORIZONTAL splitter
    topAreaSplitter->addWidget(m_drawer.get());
    topAreaSplitter->addWidget(m_editor.get());
    topAreaSplitter->setSizes({250, 750}); // Initial widths for drawer and editor

    // 5. BOTTOM AREA (Output)

    // 6. ASSEMBLE THE VERTICAL SPLITTER:
    // Add the horizontal splitter (as the top widget) and the output area (as the bottom widget).
    rightSideSplitter->addWidget(topAreaSplitter);
    rightSideSplitter->addWidget(m_outPutArea.get());
    rightSideSplitter->setSizes({500, 200}); // Initial heights for top and bottom sections

    // 7. ASSEMBLE THE MAIN LAYOUT
    // mainLayout->addWidget(controlPanel);
    mainLayout->addWidget(rightSideSplitter, 1); // The '1' stretch factor allows it to expand


    // Add layout to centralWidget
    // m_centralWidgetLayout = std::make_unique<QGridLayout>(contentArea);
    // m_centralWidgetLayout->setSpacing(0);
    // m_centralWidgetLayout->setContentsMargins(0, 0, 0, 0);
    //
    // // Add control panel to the central widget.
    // const auto centralWidgetControlPanel = new QWidget(contentArea);
    // centralWidgetControlPanel->setObjectName("ControlToolBar");
    //
    // const auto centralWidgetLayout2 = new QGridLayout(centralWidgetControlPanel);
    // centralWidgetLayout2->setSpacing(0);
    // centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);
    //
    // centralWidgetControlPanel->setFixedWidth(64);
    //
    // // panel A
    // const auto centralWidgetControlPanelA = new QWidget(contentArea);
    // const auto layoutA = new QVBoxLayout(centralWidgetControlPanelA);
    // layoutA->setSpacing(1);
    // layoutA->setContentsMargins(0, 0, 0, 0);
    //
    // m_folderButton = std::make_unique<QPushButton>("🗀", contentArea);
    // m_folderButton->setObjectName("folderButton");
    // m_folderButton->setFixedSize(40, 40);
    //
    // layoutA->addWidget(m_folderButton.get());
    //
    // // Good, to be placed in panel B
    // auto* centralWidgetControlPanelB = new QWidget(contentArea);
    // auto* layoutB = new QVBoxLayout(centralWidgetControlPanelB);
    // layoutB->setSpacing(8);
    // layoutB->setContentsMargins(16, 0, 0, 8);
    //
    // m_outputButton = std::make_unique<QPushButton>("❯_", contentArea);
    // m_outputButton->setObjectName("outputConsoleButton");
    // m_outputButton->setFixedSize(40, 40);
    // layoutB->addWidget(m_outputButton.get());
    // layoutB->setSpacing(0);
    // layoutB->setContentsMargins(0, 0, 0, 0);
    //
    // // add to central widget
    // centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignTop);
    // centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 1, 1, Qt::AlignBottom);
    //
    // const auto editorAndDrawerAreaPanel = new QWidget(contentArea);
    // m_placeHolderLayout = std::make_unique<QGridLayout>(editorAndDrawerAreaPanel);
    // m_placeHolderLayout->setSpacing(0);
    // m_placeHolderLayout->setContentsMargins(0, 0, 0, 0);
    //
    // m_centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);
    //
    // m_placeHolderLayout->addWidget(m_drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
    // m_placeHolderLayout->addWidget(m_editor.get(), 0, 3, 12, 1);
    //
    // // add main content area
    // m_centralWidgetLayout->addWidget(editorAndDrawerAreaPanel, 0, 2, 12, 12);
    // m_centralWidgetLayout->addWidget(m_outPutArea.get(), 6, 2, 6, 12);

    // Connections
    connect(m_folderButton.get(), &QPushButton::clicked, this, &FramelessWindow::updateDrawer);
    connect(m_outputButton.get(), &QPushButton::clicked, this, &FramelessWindow::onShowOutputButtonClicked);
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
    return m_editor.get();
}

PluginManager* FramelessWindow::getLangPluginManager() const
{
    return pluginManager.get();
}
