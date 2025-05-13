//
// Created by talik on 4/20/2024.
//

#ifndef IT_TOOLS_EDITORMARGIN_H
#define IT_TOOLS_EDITORMARGIN_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QProcess>
#include "CustomLabel.h"
#include "Editor.h"
#include "IconButton.h"
#include "IToolsUi.h"

class EditorMargin : public QWidget {
Q_OBJECT

private slots:

	void onExecuteWholeScriptButtonClicked();
	void onExecuteSelectedScriptButtonClicked();
	void onProcessStarted();
	void updateOutputResult(int exitCode, const QString &output, const QString &error);

public:
	explicit EditorMargin( IToolsUi *ptrParent);

	~EditorMargin() override = default;

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	void onExecuteScriptButtonClicked(QString&) const;

	IconButton *exeScript;
	IconButton *exeSelectedScript;
	IToolsUi *ptrParent;
};


#endif //IT_TOOLS_EDITORMARGIN_H
