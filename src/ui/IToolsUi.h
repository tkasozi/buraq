//
// Created by Talik Kasozi on 2/3/2024.
//
#ifndef IT_TOOLS_IT_UI_H
#define IT_TOOLS_IT_UI_H

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


class IToolsUi : public QMainWindow {
Q_OBJECT

private slots:

	void onClicked();

	void onShowOutputButtonClicked();

public:
	explicit IToolsUi(QWidget *parent = nullptr);

	~IToolsUi() override = default;

	OutputDisplay *getOutputDisplay();
	Editor *getEditor();
	QStatusBar *getQStatusBar();

private:
	CustomDrawer *drawer;
	QGridLayout *centralWidgetLayout;
	IconButton *folderButton;
	OutputDisplay *outPutArea;
	QGridLayout *placeHolderLayout;
	Editor *itoolsEditor;
	QWidget *editorAndDrawerAreaPanel;
	QWidget *editorMargin;
	ToolBar *toolBar;
	QStatusBar *statusBar;
};


#endif // IT_TOOLS_IT_UI_H
