//
// Created by talik on 3/2/2024.
//

#ifndef IT_TOOLS_LINE_NUMBER_AREA_H
#define IT_TOOLS_LINE_NUMBER_AREA_H

#include <QWidget>
#include "Editor.h"

class LineNumberArea : public QWidget {

public:

	explicit LineNumberArea(Editor *parent) : QWidget(parent), editor(parent) {
		setStyleSheet("background-color: none; border-right: 1px solid #383838; padding: 0px; margin: 0px; color: #7E7E7E;");
		setMinimumHeight(1000);
		setFixedWidth(64);
	};

	[[nodiscard]] QSize sizeHint() const override {
		return {editor->lineNumberAreaWidth(), 0};
	}
	~LineNumberArea() override = default;

protected:
	void paintEvent(QPaintEvent *event) override {
		editor->lineNumberAreaPaintEvent(event);
	}

private:
	Editor *editor;
};

#endif //IT_TOOLS_LINE_NUMBER_AREA_H
