//
// Created by talik on 5/29/2025.
//

#ifndef FRAMELESS_WINDOW_H
#define FRAMELESS_WINDOW_H

#include <QMainWindow>
#include <QWidget>

#include "settings/UserSettings.h"
#include "settings/SettingManager/SettingsManager.h"

namespace buraq
{
    struct buraq_api;
}

class QPushButton; // Forward declaration
class QStatusBar;
class QGridLayout;
class QPoint;
class QHBoxLayout;
class QVBoxLayout;
class CustomDrawer;
class PluginManager;
class OutputDisplay;
class Editor;
class EditorMargin;
class ToolBar;
class ThemeManager;

class FramelessWindow final : public QMainWindow
{
    Q_OBJECT

public:
    void init();
    explicit FramelessWindow(QWidget* parent = nullptr);
    ~FramelessWindow() override;

    [[nodiscard]] Editor* getEditor() const;
    void onShowOutputButtonClicked() const;
    [[nodiscard]] EditorMargin* getEditorMargin() const;
    [[nodiscard]] PluginManager* getLangPluginManager() const;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // Optional: If you need to draw a custom border or background for the whole m_window
    // void paintEvent(QPaintEvent *event) override;

public slots:
    void processStatusSlot(const QString&, int timeout = 5000) const;
    void processResultSlot(int exitCode, const QString& output, const QString& error) const;
    void updateDrawer() const;
    void closeWindowSlot();
    void showMaximizeOrRestoreSlot();
    void windowDrag(QMouseEvent* event);

private:
    // Helper function to update the cursor shape based on position
    void updateCursorShape(const QPoint &pos);

    // Helper function to calculate which edges the mouse is on
    Qt::Edges calculateEdges(const QPoint &pos, int margin) const;

    void initContentAreaLayout(QWidget* contentArea);

    ThemeManager& themeManager;

    std::unique_ptr<PluginManager> pluginManager;
    std::unique_ptr<CustomDrawer> m_drawer;
    std::unique_ptr<QGridLayout> m_centralWidgetLayout;
    std::unique_ptr<OutputDisplay> m_outPutArea;
    std::unique_ptr<QGridLayout> m_placeHolderLayout;
    std::unique_ptr<Editor> m_itoolsEditor;
    std::unique_ptr<EditorMargin> editorMargin;
    std::unique_ptr<ToolBar> m_toolBar;
    std::unique_ptr<QWidget> m_titleBar;
    std::unique_ptr<buraq::buraq_api> api_context;
    std::unique_ptr<FramelessWindow> m_framelessWindow;

    std::unique_ptr<QPushButton> m_folderButton;
    std::unique_ptr<QPushButton> m_outputButton;
    std::unique_ptr<QPushButton> m_settingsButton;

    QStatusBar* m_statusBar{};

    QPushButton* m_minimizeButton;
    QPushButton* m_maximizeButton;
    QPushButton* m_closeButton;

    SettingsManager settingsManager{};
    UserSettings userPreferences;

    bool m_resizing = false;
    bool m_dragging = false;
    Qt::Edges m_resizeEdges;
    int m_resizeMargin = 5; // The pixel margin to detect resizing
    QPoint m_dragPosition; // To store the offset of the mouse click from the m_window's top-left

signals:
    void closeApp();
};

#endif //FRAMELESS_WINDOW_H
