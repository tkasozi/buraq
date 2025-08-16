//
// Created by talik on 3/2/2024.
//

#ifndef EDITOR_MARGIN_H
#define EDITOR_MARGIN_H

#include <QWidget>
#include <QPainter>
#include <QPlainTextEdit>

#include "editor/CodeRunner.h"
#include "CommonWidget.h"
#include "editor/LineNumberAreaWidget.h"

class EditorMargin final : public CommonWidget {

public slots:
	void updateState(const buraq::EditorState &newState) const;
	void onEditorScrolled();
	void updateMarginWidth(const buraq::EditorState& state) const;

public:

	explicit EditorMargin(QWidget *appUi, QWidget *parent = nullptr);
	void setEditor(QPlainTextEdit *editor) { m_editor = editor; }
	~EditorMargin() override = default;

private:
	QWidget *appUi;
	std::unique_ptr<CodeRunner> codeRunner;
	std::unique_ptr<LineNumberAreaWidget> line_numbers_widget;
	QPlainTextEdit *m_editor{}; // Pointer to the associated editor

	void setupSignals() const override;
	int lineNumberAreaWidth();
};

#endif //EDITOR_MARGIN_H
