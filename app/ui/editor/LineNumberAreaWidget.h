//
// Created by talik on 5/29/2025.
//

#ifndef LINE_NUMBER_AREA_WIDGET_H
#define LINE_NUMBER_AREA_WIDGET_H


#include <QWidget>
#include <QFontMetrics> // For calculating text sizes
#include "IToolsAPI.h"

class LineNumberAreaWidget : public QWidget {
Q_OBJECT

public:
	explicit LineNumberAreaWidget(QWidget *parent = nullptr);

	// Call this when the editor's state changes to provide data for painting
	void updateEditorState(const EditorState &state);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	EditorState m_editorState{.lineHeight = 19};
// You might want to store font, colors, etc., as members
// or get them from the parent EditorMargin if needed.
};

#endif //LINE_NUMBER_AREA_WIDGET_H
