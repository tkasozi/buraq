//
// Created by talik on 5/29/2025.
//

#include "FramelessWindow.h"

#include <QStatusBar>
#include <QVBoxLayout>

#include "app_version.h"
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
#include <QPixmap>
#include <QPainter>
#include <QFont>

// Helper function to create a QIcon from a text character
QIcon createIconFromCharacter(const QString &character, const QWidget* parentWidget, const int size = 64)
{
    // Create a square, transparent pixmap to draw on
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    // Use QPainter to draw the character
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing); // For smooth edges

    // Configure the font
    QFont font;
    font.setPointSize(size * 0.6); // Adjust font size relative to pixmap size
    font.setBold(true);
    painter.setFont(font);

    const QColor textColor = parentWidget->palette().color(QPalette::WindowText);

    // Set the character color
    painter.setPen(textColor);

    // Draw the character centered on the pixmap
    painter.drawText(pixmap.rect(), Qt::AlignLeft, character);

    // The painter is automatically finished when it goes out of scope

    return QIcon(pixmap);
}

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
    const QIcon lightningIcon = createIconFromCharacter("⌁", this);

    // Set it as the window icon
    this->setWindowIcon(lightningIcon);
}

FramelessWindow::FramelessWindow(QWidget* parent)
    : QMainWindow(parent),
      themeManager(ThemeManager::instance()),
      m_toolBar(std::make_unique<ToolBar>(this)),
      m_titleBar(std::make_unique<QWidget>(this)),
      m_statusBar(new QStatusBar(this)),
      userPreferences(settingsManager.loadSettings()),
      m_dragPosition(QPoint(0, 0))
{
    init();

    // Create a central widget to hold the main layout.
    // QMainWindow requires a central widget to manage content.
    QWidget* centralWidget = new QWidget(this);

    // Main vertical layout for the whole m_window
    const auto mainLayout = new QVBoxLayout(centralWidget); // Apply layout to the central widget
    mainLayout->setContentsMargins(0, 0, 0, 0); // No margins for the main layout
    mainLayout->setSpacing(0);

    // Custom Title Bar
    const auto m_titleBar_ = m_titleBar.get();
    const auto toolbarEvents = new ToolBarEvent(m_titleBar_);
    m_titleBar_->installEventFilter(toolbarEvents);
    m_titleBar_->setFixedHeight(35); // Set your desired title bar height
    m_titleBar_->setObjectName("customTitleBar"); // For styling

    const auto titleBarLayout = new QHBoxLayout(m_titleBar_);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
    titleBarLayout->setSpacing(0);

    const auto titleText = new QLabel(m_titleBar_);
    titleText->setFixedWidth(width());
    titleText->setObjectName("titleText");

    // 1. Enable Rich Text rendering on the QLabel
    titleText->setTextFormat(Qt::RichText);

    qDebug() << "curr theme: " << themeManager.currentTheme();

    // 2. Construct the HTML string
    const QString titleHtml = QString(
        "<div style='display: flex; width: %2px; justify-content: space-between;'>" // Make the main part bold
        "<span style='font-size: 14pt;'>&nbsp;⌁</span>" // Icon with some space
        "<span style='font-size: 12pt; color: #AAAAAA;'>&nbsp; %1</span>" // Version number, smaller and lighter
        "</div>"
    ).arg(UpdateInfo().currentVersion.data(), titleText->width());

    // 3. Set the HTML content
    titleText->setText(titleHtml);

    m_minimizeButton = new QPushButton("—", m_titleBar_); // Underscore for minimize
    m_minimizeButton->setObjectName("minimizeButton"); // For specific styling

    m_maximizeButton = new QPushButton("☐", m_titleBar_); // Square for maximize/restore
    m_maximizeButton->setObjectName("maximizeButton"); // For specific styling

    m_closeButton = new QPushButton("✕", m_titleBar_); // X for close
    m_closeButton->setObjectName("closeButton"); // For specific styling

    m_minimizeButton->setFixedSize(40, m_titleBar_->height());
    m_maximizeButton->setFixedSize(40, m_titleBar_->height());
    m_closeButton->setFixedSize(40, m_titleBar_->height());

    titleBarLayout->addWidget(titleText);
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(m_minimizeButton);
    titleBarLayout->addWidget(m_maximizeButton);
    titleBarLayout->addWidget(m_closeButton);

    mainLayout->addWidget(m_titleBar_);

    // Add Tool bar
    m_toolBar->setFixedHeight(35);
    // Add the File menu first
    m_toolBar->addFileMenu();
    mainLayout->addWidget(m_toolBar.get());

    // Content Area (example)
    const auto contentArea = new QWidget(this);
    initContentAreaLayout(contentArea);
    mainLayout->addWidget(contentArea, 1); // Content area takes remaining space

    // Set the central widget for the QMainWindow
    setCentralWidget(centralWidget);

    // Status bar
    m_statusBar->setObjectName("appStatusBar");
    m_statusBar->setSizeGripEnabled(false);
    setStatusBar(m_statusBar); // Use QMainWindow's dedicated function for the status bar

    // Connections
    connect(m_maximizeButton, &QPushButton::clicked, this, &FramelessWindow::showMaximizeOrRestoreSlot);
    connect(m_minimizeButton, &QPushButton::clicked, this, &FramelessWindow::showMinimized);
    connect(m_closeButton, &QPushButton::clicked, this, &FramelessWindow::close);
    connect(this, &FramelessWindow::closeApp, this, &FramelessWindow::close);
    connect(toolbarEvents, &ToolBarEvent::showMaximizedOrRestore, this, &FramelessWindow::showMaximizeOrRestoreSlot);
    connect(toolbarEvents, &ToolBarEvent::dragWindow, this, &FramelessWindow::windowDrag);
}

// smart pointers will be cleaned up by std::unique_ptr
FramelessWindow::~FramelessWindow()
{
    // save the last window size & position
    userPreferences.windowSize = this->size();
    userPreferences.windowPosition = m_dragPosition;
    settingsManager.saveSettings(userPreferences);
};

void FramelessWindow::windowDrag(QMouseEvent* event)
{
    m_dragPosition = event->globalPosition().toPoint();
    move(m_dragPosition);
    event->accept();
}


Qt::Edges FramelessWindow::calculateEdges(const QPoint &pos, const int margin) const
{
    Qt::Edges edges;
    if (pos.x() < margin) edges |= Qt::LeftEdge;
    if (pos.x() > width() - margin) edges |= Qt::RightEdge;
    if (pos.y() < margin) edges |= Qt::TopEdge;
    if (pos.y() > height() - margin) edges |= Qt::BottomEdge;
    return edges;
}

void FramelessWindow::updateCursorShape(const QPoint &pos)
{
    if (m_resizing)
        return;

    m_resizeEdges = calculateEdges(pos, m_resizeMargin);

    if (m_resizeEdges == (Qt::TopEdge | Qt::LeftEdge) || m_resizeEdges == (Qt::BottomEdge | Qt::RightEdge))
        setCursor(Qt::SizeFDiagCursor);
    else if (m_resizeEdges == (Qt::TopEdge | Qt::RightEdge) || m_resizeEdges == (Qt::BottomEdge | Qt::LeftEdge))
        setCursor(Qt::SizeBDiagCursor);
    else if (m_resizeEdges & (Qt::LeftEdge | Qt::RightEdge))
        setCursor(Qt::SizeHorCursor);
    else if (m_resizeEdges & (Qt::TopEdge | Qt::BottomEdge))
        setCursor(Qt::SizeVerCursor);
    else
        setCursor(Qt::ArrowCursor);
}

void FramelessWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_resizeEdges != 0)
        {
            m_resizing = true;
            m_dragPosition = event->globalPosition().toPoint();
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void FramelessWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (m_resizing)
    {
        const QPoint currentPos = event->globalPosition().toPoint();
        const QPoint delta = currentPos - m_dragPosition;
        QRect newGeometry = geometry();

        if (m_resizeEdges & Qt::LeftEdge) newGeometry.setLeft(newGeometry.left() + delta.x());
        if (m_resizeEdges & Qt::RightEdge) newGeometry.setRight(newGeometry.right() + delta.x());
        if (m_resizeEdges & Qt::TopEdge) newGeometry.setTop(newGeometry.top() + delta.y());
        if (m_resizeEdges & Qt::BottomEdge) newGeometry.setBottom(newGeometry.bottom() + delta.y());

        if(newGeometry.width() < minimumWidth()) newGeometry.setLeft(geometry().left());
        if(newGeometry.height() < minimumHeight()) newGeometry.setTop(geometry().top());

        setGeometry(newGeometry);
        m_dragPosition = currentPos;
    }
    else if (m_dragging)
    {
        move(event->globalPosition().toPoint() - m_dragPosition);
    }
    else
    {
        updateCursorShape(event->pos());
    }
    QMainWindow::mouseMoveEvent(event);
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragging = false;
        m_resizing = false;
        setCursor(Qt::ArrowCursor);
    }
    QMainWindow::mouseReleaseEvent(event);
}

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

void FramelessWindow::initContentAreaLayout(QWidget* contentArea)
{
    // Add layout to centralWidget
    m_centralWidgetLayout = std::make_unique<QGridLayout>(contentArea);
    m_centralWidgetLayout->setSpacing(0);
    m_centralWidgetLayout->setContentsMargins(0, 0, 0, 0);

    // // Setting m_window title and docking icon TODO---remove?
    // setWindowTitle(UpdateInfo().currentVersion.data());
    // setWindowIcon(Config::singleton().getAppLogo());

    // Add control panel to the central widget.
    const auto centralWidgetControlPanel = new QWidget(contentArea);
    centralWidgetControlPanel->setObjectName("ControlToolBar");

    const auto centralWidgetLayout2 = new QGridLayout(centralWidgetControlPanel);
    centralWidgetLayout2->setSpacing(0);
    centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);

    centralWidgetControlPanel->setFixedWidth(64);

    // panel A
    auto* layoutA = new QVBoxLayout();
    layoutA->setSpacing(1);
    layoutA->setContentsMargins(0, 0, 0, 0);

    auto* centralWidgetControlPanelA = new QWidget(this);
    centralWidgetControlPanelA->setLayout(layoutA);

    centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignTop);

    m_folderButton = std::make_unique<QPushButton>("🗀", this);
    m_folderButton->setObjectName("folderButton");
    m_folderButton->setFixedSize(40, 40);

    layoutA->addWidget(m_folderButton.get());

    // Good, to be placed in panel B
    auto* centralWidgetControlPanelB = new QWidget(this);
    auto* layoutB = new QVBoxLayout(centralWidgetControlPanelB);
    layoutB->setSpacing(8);
    layoutB->setContentsMargins(16, 0, 0, 8);

    centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 1, 1, Qt::AlignBottom);

    m_outputButton = std::make_unique<QPushButton>("❯_", this);
    m_outputButton->setObjectName("outputConsoleButton");
    m_outputButton->setFixedSize(40, 40);
    connect(m_outputButton.get(), &QPushButton::clicked, this, &FramelessWindow::onShowOutputButtonClicked);
    layoutB->addWidget(m_outputButton.get());

    m_settingsButton = std::make_unique<QPushButton>("⋮", this);
    m_settingsButton->setObjectName("settingGearButton");
    m_settingsButton->setFixedSize(40, 40);

    // Create an instance of your settings dialog
    const auto settingsDialog = new SettingsDialog(this);
    // Connect the button's click signal to open the dialog
    connect(m_settingsButton.get(), &QPushButton::clicked, settingsDialog, &SettingsDialog::exec);

    layoutB->addWidget(m_settingsButton.get());

    // add to central widget
    m_centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);

    // Component
    connect(m_folderButton.get(), &QPushButton::clicked, this, &FramelessWindow::updateDrawer);

    // layout c
    layoutB->setSpacing(0);
    layoutB->setContentsMargins(0, 0, 0, 0);

    // Editor helper component.
    m_itoolsEditor = std::make_unique<Editor>(this);

    // Handles file nav
    m_drawer = std::make_unique<CustomDrawer>(m_itoolsEditor.get());
    // This where the output_display generated after executing the script will be displayed
    m_outPutArea = std::make_unique<OutputDisplay>(this);

    const auto editorAndDrawerAreaPanel = new QWidget(this);
    m_placeHolderLayout = std::make_unique<QGridLayout>(editorAndDrawerAreaPanel);
    m_placeHolderLayout->setSpacing(0);
    m_placeHolderLayout->setContentsMargins(0, 0, 0, 0);

    m_placeHolderLayout->addWidget(m_drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
    m_placeHolderLayout->addWidget(m_itoolsEditor.get(), 0, 3, 12, 1);

    // add main content area
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
