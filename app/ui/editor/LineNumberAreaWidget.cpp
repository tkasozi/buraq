//
// Created by talik on 5/29/2025.
//

#include "LineNumberAreaWidget.h"
#include <QPainter>
#include <QPalette> // For theme colors

LineNumberAreaWidget::LineNumberAreaWidget(QWidget *parent) : QWidget(parent) {
	// It's good to set an initial size policy or minimum width
	setMinimumWidth(5);
}

void LineNumberAreaWidget::updateEditorState(const EditorState &state) {
	if (m_editorState != state) { // Basic check to avoid unnecessary updates
		m_editorState = state;
		update(); // This is KEY: it schedules a call to paintEvent()
	}
}

void LineNumberAreaWidget::paintEvent(QPaintEvent *event) {
	Q_UNUSED(event); // We are redrawing the whole thing based on m_editorState

	// Create a QPainter that is active for THIS widget.
	// It automatically begins and ends.
	QPainter painter(this);

	// Use this widget's fontMetrics
	const int lineHeight =  std::max(19, m_editorState.lineHeight);

	int currentY = 0; // Start Y position from the top. Add padding if needed.

	// Loop to draw line numbers.
	for (int block_num_to_display = 1; block_num_to_display <= m_editorState.blockCount; ++block_num_to_display) {
		QRect lineAreaRect(0, currentY, width(), lineHeight); // Use this widget's width()

		if (currentY >= height()) { // Use this widget's height()
			break; // Stop if drawing outside the widget's visible area
		}

		// Highlight the active line
		// Ensure m_editorState.cursorBlockNumber is 0-indexed if comparing with (block_num_to_display - 1)
		if (m_editorState.isSelected) {
			auto it = std::find(
					m_editorState.selectedBlockNumbers.begin(),
					m_editorState.selectedBlockNumbers.end(),
					(block_num_to_display - 1)
			);

			if (it != m_editorState.selectedBlockNumbers.end()) {
				// value is found.
				painter.fillRect(lineAreaRect, QColor(Qt::cyan).lighter(25));
			} else {
				// ignore
			}
		} else if (m_editorState.cursorBlockNumber == (block_num_to_display - 1)) {
			painter.fillRect(lineAreaRect, QColor(Qt::lightGray).lighter(25));
		}

		QString numberText = QString::number(block_num_to_display);
		int textPaddingLeft = 4;
		QRect textDrawingRect = lineAreaRect;
		textDrawingRect.setLeft(lineAreaRect.left() + textPaddingLeft);

		painter.setPen(palette().color(QPalette::Light)); // Use a color from the widget's palette
		painter.drawText(textDrawingRect, Qt::AlignLeft | Qt::AlignVCenter, numberText);

		currentY += lineHeight;
	}
	// qDebug() << "LineNumberAreaWidget::paintEvent finished.";
}
