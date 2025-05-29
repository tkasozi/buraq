//
// Created by talik on 5/28/2025.
//
#include "EditorMargin.h"
#include <QHBoxLayout> // Required for QHBoxLayout

EditorMargin::EditorMargin(QWidget *appUi) : CommonWidget(nullptr), appUi(appUi) {
	setStyleSheet("border-right: 1px solid #383838; padding: 0px; margin: 0px; color: #7E7E7E;");
	setMinimumHeight(1000);
	setFixedWidth(82); // The EditorMargin itself has a fixed width

	// No need to explicitly call `setLayout(main_layout)` if `main_layout`
	// was constructed with `this` as its parent: `new QHBoxLayout(this)`.
	// This will be the main horizontal layout for EditorMargin
	auto main_layout = new QHBoxLayout(this); // Passing 'this' sets the layout on EditorMargin
	main_layout->setSpacing(0);
	main_layout->setContentsMargins(0, 0, 0, 0);

	// --- First Widget: Line Numbers Area ---
	// This widget will be for line numbers
	// Ensure line_numbers is a QWidget. If it's a custom class deriving QWidget, that's fine.
	// If it's just a raw QWidget that you intend to draw on, that's also fine.
	line_numbers = std::make_unique<QWidget>(appUi); // Parenting to editorPtr as in original

	// Layout for the line_numbers widget (e.g., if you add more items to it later)
	auto line_number_internal_layout = new QVBoxLayout(line_numbers.get()); // Set this layout on the line_numbers widget
	line_number_internal_layout->setSpacing(0);
	line_number_internal_layout->setContentsMargins(0, 0, 0, 0);
	// If line_numbers is a custom widget that handles its own content/painting,
	// it might not even need an internal layout unless you're adding child widgets to it.
	// If it's just a placeholder for custom painting, no child widgets or internal layout might be needed.

	// Add the line_numbers widget to the main horizontal layout
	main_layout->addWidget(line_numbers.get(), 1); // Example stretch factor, adjust as needed (e.g., 0 for default, 1 for some stretch)

	// --- Second Widget: Action Area (contains CodeRunner) ---
	// This widget will hold the codeRunner
	auto action_widget = new QWidget(this); // Parent to EditorMargin

	auto action_layout = new QVBoxLayout(action_widget); // Layout for the action_widget
	action_layout->setSpacing(0);
	action_layout->setContentsMargins(0, 0, 0, 0);

	codeRunner = std::make_unique<CodeRunner>(appUi); // Assuming CodeRunner is a QWidget or QWidget-derived
	action_layout->addWidget(codeRunner.get());

	// Then, add a stretchable space after it
	action_layout->addStretch(1);

	// Add the action_widget to the main horizontal layout
	main_layout->addWidget(action_widget); // Default stretch factor

	EditorMargin::setupSignals();
}

void EditorMargin::lineNumberAreaPaintEvent(QPaintEvent *event) {
//	QPainter painter(lineNumberArea.get());
//	painter.setBackgroundMode(Qt::BGMode::TransparentMode);
//
//	QTextBlock block = firstVisibleBlock();
//	int blockNumber = block.blockNumber();
//	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
//	int bottom = top + qRound(blockBoundingRect(block).height());
//
//	QString editorText = toPlainText();
//	// don't set line numbers until the placeholder text is cleared.
//	while (block.isValid() && top <= event->rect().bottom() && !editorText.isEmpty()) {
//		if (block.isVisible() && bottom >= event->rect().top()) {
//
//			int i = blockNumber + 1;
//			QString number = QString::number(i);
//
//			// Set background color
//			QRect rect(0, top, lineNumberArea->width(), fontMetrics().height());
//			QColor lineColor = QColor(Qt::lightGray).lighter(25);
//
//			// Highlight the line number that is active
//			QTextCursor cursor = textCursor();
//			if (cursor.blockNumber() == block.blockNumber()) {
//				painter.fillRect(rect, lineColor);  // Fill with light gray
//			}
//
//			// Adds a margin-left to the line rectangle
//			rect.setLeft(4);
//
//			painter.drawText(rect, Qt::AlignLeft, number);
//		}
//
//		block = block.next();
//		top = bottom;
//		bottom = top + qRound(blockBoundingRect(block).height());
//		++blockNumber;
//	}
}

void EditorMargin::paintEvent(QPaintEvent *event) {
	QWidget::paintEvent(event);
}

void EditorMargin::setupSignals() const {

}
