//
// Created by talik on 3/2/2024.
//

#ifndef EDITOR_MARGIN_H
#define EDITOR_MARGIN_H

#include <QWidget>
#include <QPainter>
#include "editor/Editor.h"
#include "editor/CodeRunner.h"
#include "CommonWidget.h"
#include "editor/LineNumberAreaWidget.h"

class EditorMargin : public CommonWidget {

public slots:
	void updateState(const EditorState &newState);

public:

	explicit EditorMargin(QWidget *appUi);

	~EditorMargin() override = default;

private:
	QWidget *appUi;
	std::unique_ptr<CodeRunner> codeRunner;
	std::unique_ptr<LineNumberAreaWidget> line_numbers_widget;

	void setupSignals() const override;
	int lineNumberAreaWidth();
};

#endif //EDITOR_MARGIN_H
