// toolbar.h
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>

struct IWindow;
class QWidget;
class QMenu;
class QIcon;
class QAction;
class QPushButton;

class ToolBar final : public QToolBar
{
	Q_OBJECT

public:
	explicit ToolBar(const QString &title, QWidget *parent = nullptr);
	explicit ToolBar(QWidget *parent = nullptr);

	void addCustomAction(const QString &text, const QIcon &icon = QIcon());
	void addFileMenu();
	QWidget* m_window;

	signals:
		void customActionTriggered();
	void newFileTriggered();
	void openFileTriggered();
	void saveFileTriggered();
	void exitTriggered();

private slots:
	void onCustomActionTriggered();
	void onNewFile();
	void onOpenFile();
	void onSaveFile();
	void onExit();
	void onFileMenuButtonClicked(); // Add this new slot declaration

private:
	QAction *m_customAction;
	QMenu *m_fileMenu;
    QPushButton* m_fileMenuButton;
};

#endif // TOOLBAR_H
