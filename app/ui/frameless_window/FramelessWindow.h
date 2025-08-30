//
// Created by talik on 5/29/2025.
//

#ifndef FRAMELESS_WINDOW_H
#define FRAMELESS_WINDOW_H

#include <QMainWindow>
#include <QWidget>

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

class FramelessWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit FramelessWindow(QWidget* parent = nullptr);
    ~FramelessWindow() override;

    [[nodiscard]] Editor* getEditor() const;
    void onShowOutputButtonClicked() const;
    [[nodiscard]] EditorMargin* getEditorMargin() const;
    [[nodiscard]] PluginManager* getLangPluginManager() const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // Optional: If you need to draw a custom border or background for the whole m_window
    // void paintEvent(QPaintEvent *event) override;

public slots:
    void processStatusSlot(const QString&, int timeout = 5000) const;
    void processResultSlot(int exitCode, const QString& output, const QString& error) const;
    void updateDrawer() const;
    void closeWindowSlot();

private:
    void initContentAreaLayout(QWidget* contentArea);

    std::unique_ptr<PluginManager> pluginManager;
    std::unique_ptr<CustomDrawer> m_drawer;
    std::unique_ptr<QGridLayout> m_centralWidgetLayout;
    std::unique_ptr<OutputDisplay> m_outPutArea;
    std::unique_ptr<QGridLayout> m_placeHolderLayout;
    std::unique_ptr<Editor> m_itoolsEditor;
    std::unique_ptr<EditorMargin> editorMargin;
    std::unique_ptr<ToolBar> m_toolBar;
    std::unique_ptr<buraq::buraq_api> api_context;
    std::unique_ptr<FramelessWindow> m_framelessWindow;

    std::unique_ptr<QPushButton> m_folderButton;
    std::unique_ptr<QPushButton> m_outputButton;
    std::unique_ptr<QPushButton> m_settingsButton;

    QStatusBar* m_statusBar{};

    QWidget* m_titleBar{};
    QPushButton* m_minimizeButton;
    QPushButton* m_maximizeButton;
    QPushButton* m_closeButton;

    bool m_dragging;
    QPoint m_dragPosition; // To store the offset of the mouse click from the m_window's top-left

signals:
    void closeApp();
};

#endif //FRAMELESS_WINDOW_H
