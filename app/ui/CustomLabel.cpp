//
// Created by talik on 4/19/2024.
//

#include "CustomLabel.h"

CustomLabel::CustomLabel(QWidget *parent): QLabel(parent) {
	setMouseTracking(true);
}

bool CustomLabel::event(QEvent *event) {
	if (event->type() == QEvent::MouseButtonRelease) {
		// Handle mouse click event here (e.g., emit a custom signal)
		emit clicked();
		return true; // Stop event propagation (optional)
	}
	return QLabel::event(event);
}