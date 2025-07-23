// toolbar.h
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDebug>
#include <QMenu>
#include <QPushButton> // Add this include

class ToolBar : public QToolBar
{
	Q_OBJECT

public:
	explicit ToolBar(const QString &title, QWidget *parent = nullptr);
	explicit ToolBar(QWidget *parent);

	void addCustomAction(const QString &text, const QIcon &icon = QIcon());
	void addFileMenu();

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
};

#endif // TOOLBAR_H
