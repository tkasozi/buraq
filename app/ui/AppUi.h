//
// Created by Talik Kasozi on 2/3/2024.
//
#ifndef APP_UI_H
#define APP_UI_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QObject>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>

#include "ToolBarEventFilter.h"
#include "ToolButton.h"
#include "CustomDrawer.h"
#include "IconButton.h"
#include "OutputDisplay.h"
#include "ToolBar.h"
#include "../PluginManager.h"


class AppUi : public QMainWindow {
Q_OBJECT

private slots:

	void onClicked();

	void onShowOutputButtonClicked();

public:
	explicit AppUi(QWidget *parent = nullptr);

	// The std::unique_ptr members will automatically
	// delete their managed objects when AppUi is destroyed.
	// No manual delete, no manual nullptr.
	~AppUi() override = default;

	OutputDisplay *getOutputDisplay();

	Editor *getEditor();

	QStatusBar *getQStatusBar();

	PluginManager *getPluginManager();

private:
	std::unique_ptr<PluginManager> pluginManager;
	std::unique_ptr<CustomDrawer> drawer;
	std::unique_ptr<QGridLayout> centralWidgetLayout;
	std::unique_ptr<IconButton> folderButton;
	std::unique_ptr<OutputDisplay> outPutArea;
	std::unique_ptr<QGridLayout> placeHolderLayout;
	std::unique_ptr<Editor> itoolsEditor;
	std::unique_ptr<QWidget> editorAndDrawerAreaPanel;
	std::unique_ptr<QWidget> editorMargin;
	std::unique_ptr<ToolBar> toolBar;
	std::unique_ptr<QStatusBar> statusBar;
};


#endif // APP_UI_H
