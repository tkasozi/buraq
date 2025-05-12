//
// Created by talik on 2/13/2024.
//

#ifndef IT_TOOLS_TOOLBAR_H
#define IT_TOOLS_TOOLBAR_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>

#include "ToolButton.h"
#include "ToolBarEventFilter.h"
#include "../utils/Config.h"

class ToolBar : public QWidget {
Q_OBJECT

private:
	void configureLayout();

	Config appConfig;

	// toolbar buttons and versionLabel
	ToolButton *maxRestoreBtn;
	ToolButton *minimizeBtn;
	ToolButton *closeBtn;
	QLabel *versionLabel;

private slots:

	void onCloseWindowButtonClicked();

	void onMinimizeWindowButtonClicked();

	void onMaximizeRestoreWindowButtonClicked();

public:
	explicit ToolBar(QWidget *parent = nullptr);
	void updateMaxAndRestoreIconButton();
	~ToolBar() override = default;
};


#endif //IT_TOOLS_TOOLBAR_H
