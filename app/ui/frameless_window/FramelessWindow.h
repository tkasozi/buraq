//
// Created by talik on 5/29/2025.
//

#ifndef FRAMELESS_WINDOW_H
#define FRAMELESS_WINDOW_H

#include <QWidget>
#include <QPoint>

class QPushButton; // Forward declaration
class QHBoxLayout;
class QVBoxLayout;

class FramelessWindow : public QWidget {
Q_OBJECT

public:
	explicit FramelessWindow(QWidget *parent = nullptr);

protected:
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	// Optional: If you need to draw a custom border or background for the whole window
	// void paintEvent(QPaintEvent *event) override;

private slots:
	void handleMinimize();
	void handleMaximizeRestore();
	void handleClose();

private:
	QWidget* m_titleBar;
	QPushButton* m_minimizeButton;
	QPushButton* m_maximizeButton;
	QPushButton* m_closeButton;

	bool m_dragging;
	QPoint m_dragPosition; // To store the offset of the mouse click from the window's top-left
};

#endif //FRAMELESS_WINDOW_H
