// toolbar.cpp
#include "toolbar.h"
#include <QMessageBox>
#include <QByteArray>
#include <QBuffer>
#include <QPixmap>
#include <QPushButton> // Required for QPushButton

// --- Base64 encoded PNG icon data (16x16 pixel placeholders) ---
// ... (icon data remains the same) ...
// Green 'N' for New
const char* ICON_NEW_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAP0lEQVR42mNkYGD4z0AFIwMDw0/AwPAXlAwMDL8jYGBgYGBgYGBgAAAtEw/5/17+P+f/x8YGBgYGBgAAAEcBQH/tq2IAAAAAElFTkSuQmCC";
// Blue 'O' for Open
const char* ICON_OPEN_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAP0lEQVR42mNkYGD4z0AFIwMDw0/AwPAXlAwMDL8jYGBgYGBgYGBgAAAtEw/5/17+P+f/x8YGBgYGBgAAAEcBQH/tq2IAAAAAElFTgRUQmCC";
// Yellow 'S' for Save
const char* ICON_SAVE_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAP0lEQVR42mNkYGD4z0AFIwMDw0/AwPAXlAwMDL8jYGBgYGBgYGBgAAAtEw/5/17+P+f/x8YGBgYGBgAAAEcBQH/tq2IAAAAAElFTkSuQmCC";
// Red 'X' for Exit
const char* ICON_EXIT_BASE64 = "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAP0lEQVR42mNkYGD4z0AFIwMDw0/AwPAXlAwMDL8jYGBgYGBgYGBgAAAtEw/5/17+P+f/x8YGBgYGBgAAAEcBQH/tq2IAAAAAElFTkSuQmCC";
// Orange 'F' for File Menu
const char* ICON_FILE_MENU_BASE64 = "iVBORw0KGgoAAAANsXwAAAABJRU5ErkJggg==";
// Purple 'P' for Print
const char* ICON_PRINT_BASE64 = "iVBORw0KGgoAAAANsXwAAAABJRU5ErkJggg==";
// Gray 'H' for Help
const char* ICON_HELP_BASE64 = "iVBORw0KGgoAAAANsXwAAAABJRU5ErkJggg==";


// Helper function to create QIcon from Base64 data
QIcon createIconFromBase64(const char* base64Data) {
    QByteArray imageData = QByteArray::fromBase64(base64Data);
    QPixmap pixmap;
    pixmap.loadFromData(imageData, "PNG"); // Specify PNG format
    return QIcon(pixmap);
}

// Constructor with title
ToolBar::ToolBar(const QString &title, QWidget *parent)
    : QToolBar(title, parent)
{
    m_customAction = nullptr;
    m_fileMenu = nullptr;

    // --- CHANGE THESE LINES ---
    setMovable(false);  // Disable dragging
    setFloatable(false); // Disable detaching and floating
    // --- END CHANGES ---

    setContextMenuPolicy(Qt::PreventContextMenu);
    setIconSize(QSize(24, 24));

    qDebug() << "Custom ToolBar created with title:" << title;
}

// ... (rest of the code remains the same) ...

void ToolBar::addCustomAction(const QString &text, const QIcon &icon)
{
    // Use the provided icon, or a default empty icon if none is given
    QAction *action = new QAction(icon.isNull() ? QIcon() : icon, text, this);
    addAction(action);
    connect(action, &QAction::triggered, this, &ToolBar::onCustomActionTriggered);
    qDebug() << "Custom action added to toolbar: " << text;
}

void ToolBar::onCustomActionTriggered()
{
    qDebug() << "Custom action was triggered!";
    emit customActionTriggered();
}

void ToolBar::addFileMenu()
{
    if (m_fileMenu) {
        qDebug() << "File menu already exists, not adding again.";
        return;
    }

    m_fileMenu = new QMenu("File", this); // The QMenu itself still exists

    QAction *newAction = new QAction(createIconFromBase64(ICON_NEW_BASE64), "New", this);
    QAction *openAction = new QAction(createIconFromBase64(ICON_OPEN_BASE64), "Open...", this);
    QAction *saveAction = new QAction(createIconFromBase64(ICON_SAVE_BASE64), "Save", this);
    QAction *exitAction = new QAction(createIconFromBase64(ICON_EXIT_BASE64), "Exit", this);

    m_fileMenu->addAction(newAction);
    m_fileMenu->addAction(openAction);
    m_fileMenu->addAction(saveAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(exitAction);

    // --- START CHANGES FOR QPushButton ---
    // Create a QPushButton to act as the menu trigger
    QPushButton *fileMenuButton = new QPushButton("File", this);
    // fileMenuButton->setFixedHeight(35);
    fileMenuButton->setFixedWidth(30);
    // fileMenuButton->setIcon(createIconFromBase64(ICON_FILE_MENU_BASE64)); // Set icon
    fileMenuButton->setToolTip("File Operations"); // Optional: Add a tooltip

    // Add the button to the toolbar. By default, it will be on the left if added first.
    addWidget(fileMenuButton);

    // Connect the button's clicked signal to a slot that shows the menu
    connect(fileMenuButton, &QPushButton::clicked, this, &ToolBar::onFileMenuButtonClicked);
    // --- END CHANGES FOR QPushButton ---

    connect(newAction, &QAction::triggered, this, &ToolBar::onNewFile);
    connect(openAction, &QAction::triggered, this, &ToolBar::onOpenFile);
    connect(saveAction, &QAction::triggered, this, &ToolBar::onSaveFile);
    connect(exitAction, &QAction::triggered, this, &ToolBar::onExit);

    qDebug() << "File menu successfully added to toolbar.";
}

void ToolBar::onNewFile()
{
    qDebug() << "New File action triggered!";
    QMessageBox::information(this, "File Action", "New File selected!");
    emit newFileTriggered();
}

void ToolBar::onOpenFile()
{
    qDebug() << "Open File action triggered!";
    QMessageBox::information(this, "File Action", "Open File selected!");
    emit openFileTriggered();
}

void ToolBar::onSaveFile()
{
    qDebug() << "Save File action triggered!";
    QMessageBox::information(this, "File Action", "Save File selected!");
    emit saveFileTriggered();
}

void ToolBar::onExit()
{
    qDebug() << "Exit action triggered!";
    QMessageBox::information(this, "File Action", "Exit selected! (Application would typically quit)");
    emit exitTriggered();
}

// --- NEW SLOT IMPLEMENTATION FOR QPushButton ---
void ToolBar::onFileMenuButtonClicked()
{
    // Get the sender of the signal (which is the QPushButton in this case)
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        // Map the button's bottom-left corner to global screen coordinates
        // This makes the menu appear directly below the button.
        QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
        m_fileMenu->popup(pos); // Show the menu at the calculated position
    }
}
// --- END NEW SLOT IMPLEMENTATION ---
