//
// Created by talik on 5/29/2025.
//

#include "FramelessWindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLabel> // For an example title

FramelessWindow::FramelessWindow(QWidget *parent)
		: QWidget(parent), m_dragging(false) {
	setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
	// setAttribute(Qt::WA_TranslucentBackground); // If you want non-rectangular shapes

	// Main vertical layout for the whole window
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0); // No margins for the main layout
	mainLayout->setSpacing(0);

	// Custom Title Bar
	m_titleBar = new QWidget(this);
	m_titleBar->setFixedHeight(35); // Set your desired title bar height
	m_titleBar->setObjectName("customTitleBar"); // For styling
	m_titleBar->setStyleSheet(
			"#customTitleBar { background-color: #2C3E50; border-bottom: 1px solid #1A242F; }"
			"QPushButton { background-color: transparent; color: white; border: none; font-size: 14px; padding: 5px; }"
			"QPushButton:hover { background-color: #34495E; }"
			"QPushButton#closeButton:hover { background-color: #E74C3C; }" // Specific hover for close
			"QLabel#titleText { color: white; padding-left: 10px; font-weight: bold; }"
	);


	QHBoxLayout *titleBarLayout = new QHBoxLayout(m_titleBar);
	titleBarLayout->setContentsMargins(0, 0, 0, 0);
	titleBarLayout->setSpacing(0);

	QLabel *titleText = new QLabel("My Frameless Window", m_titleBar);
	titleText->setObjectName("titleText");

	m_minimizeButton = new QPushButton("—", m_titleBar); // Underscore for minimize
	m_maximizeButton = new QPushButton("☐", m_titleBar); // Square for maximize/restore
	m_closeButton = new QPushButton("✕", m_titleBar);    // X for close
	m_closeButton->setObjectName("closeButton"); // For specific styling

	m_minimizeButton->setFixedSize(40, m_titleBar->height());
	m_maximizeButton->setFixedSize(40, m_titleBar->height());
	m_closeButton->setFixedSize(40, m_titleBar->height());

	titleBarLayout->addWidget(titleText);
	titleBarLayout->addStretch();
	titleBarLayout->addWidget(m_minimizeButton);
	titleBarLayout->addWidget(m_maximizeButton);
	titleBarLayout->addWidget(m_closeButton);

	// Content Area (example)
	QWidget *contentArea = new QWidget(this);
	contentArea->setStyleSheet("background-color: #ECF0F1;"); // Example content background
	QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
	QLabel *contentLabel = new QLabel("Frameless Window Content Area", contentArea);
	contentLabel->setAlignment(Qt::AlignCenter);
	contentLayout->addWidget(contentLabel);

	mainLayout->addWidget(m_titleBar);
	mainLayout->addWidget(contentArea, 1); // Content area takes remaining space

	setLayout(mainLayout);

	// Connections
	connect(m_minimizeButton, &QPushButton::clicked, this, &FramelessWindow::handleMinimize);
	connect(m_maximizeButton, &QPushButton::clicked, this, &FramelessWindow::handleMaximizeRestore);
	connect(m_closeButton, &QPushButton::clicked, this, &FramelessWindow::handleClose);

	resize(800, 600); // Set an initial size
}

void FramelessWindow::handleMinimize() {
	this->showMinimized();
}

void FramelessWindow::handleMaximizeRestore() {
	if (this->isMaximized()) {
		this->showNormal();
		m_maximizeButton->setText("☐"); // Restore symbol
	} else {
		this->showMaximized();
		m_maximizeButton->setText("❐"); // Actual maximize symbol (might need specific font or icon)
	}
}

void FramelessWindow::handleClose() {
	this->close();
}

void FramelessWindow::mousePressEvent(QMouseEvent *event) {
	// Check if the press is on the title bar area (but not on the buttons)
	if (event->button() == Qt::LeftButton && m_titleBar->geometry().contains(event->pos())) {
		// Check if the click was on any of the buttons
		bool onButton = false;
		for (QPushButton* btn : {m_minimizeButton, m_maximizeButton, m_closeButton}) {
			if (btn->geometry().contains(m_titleBar->mapFromParent(event->pos()))) {
				onButton = true;
				break;
			}
		}
		if (!onButton) {
			m_dragging = true;
			m_dragPosition = event->globalPos() - frameGeometry().topLeft();
			event->accept();
			return;
		}
	}
	QWidget::mousePressEvent(event); // Pass on to other handlers or base class
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *event) {
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		move(event->globalPos() - m_dragPosition);
		event->accept();
	} else {
		QWidget::mouseMoveEvent(event);
	}
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_dragging = false;
	}
	QWidget::mouseReleaseEvent(event);
}

// If you need to draw a custom border around the entire window:
// void FramelessWindow::paintEvent(QPaintEvent *event) {
//     Q_UNUSED(event);
//     QPainter painter(this);
//     QPen borderPen(QColor("#1A242F")); // Color of your border
//     borderPen.setWidth(1); // Border width
//     painter.setPen(borderPen);
//     painter.drawRect(rect().adjusted(0, 0, -1, -1)); // Draw inside the widget rect
// }