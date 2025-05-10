//
// Created by talik on 3/9/2024.
//

#ifndef IT_TOOLS_CUSTOMDRAWER_H
#define IT_TOOLS_CUSTOMDRAWER_H


#include <QWidget>
#include <QGridLayout>
#include "IconButton.h"
#include "Editor.h"
#include "FilePathLabel.h"

struct drawerState {
	QWidget *activeFileLabel;
};

class CustomDrawer : public QWidget {
Q_OBJECT

	void setActive(QWidget *pLabel);

private slots:

	void onAddButtonClicked();

	void onFileLabelClick();

public:
	enum DrawerMeasurements {
		width = 256,
	};

	explicit CustomDrawer(Editor *editorPtr, QWidget *parent = nullptr);

	void toggle();

	~CustomDrawer() override = default;

	void showPreviouslyOpenedFiles();

private:
	Editor *editor;
	IconButton *addScript;
	QVBoxLayout *pLayout;
	struct drawerState state = {.activeFileLabel = nullptr};

	static void openFilePath(FilePathLabel *label, const QString &filePath, const QString &fileName);

	void createFileLabel(const QString &filePath, const QString &fileName, bool shouldAutoOpenFile) const;
};


#endif //IT_TOOLS_CUSTOMDRAWER_H
