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


class AppUi : public QMainWindow {
Q_OBJECT

private slots:

	void onClicked();

	void onShowOutputButtonClicked();

public:
	explicit AppUi(QWidget *parent = nullptr);

	~AppUi() override = default;

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


#endif // APP_UI_H
