//QMouseEvent *
// Created by talik on 2/8/2024.
//

#ifndef IT_TOOLS_TOOL_BUTTON_H
#define IT_TOOLS_TOOL_BUTTON_H

#include <QToolButton>
#include <QObject>
#include <QMouseEvent>
#include <QEvent>
#include <QStyleFactory>

#include "itools_utils.h"


class ToolButton : public QToolButton {
Q_OBJECT

private:
	QString customHoverColor;

	void applyIconStyles(const QIcon &icon) {
		if (!icon.isNull()) {
			ToolButton::setIcon(icon);
		}
		setFixedWidth(48);
		setFixedHeight(32);
		setMouseTracking(true);
	}

private slots:

public:
	explicit ToolButton(const QString &fileName, QString customHoverColor = nullptr)
			: customHoverColor(std::move(customHoverColor)) {
		applyIconStyles(QIcon::fromTheme(fileName));
	};

	explicit ToolButton(const QIcon &icon, QString customHoverColor = nullptr)
			: customHoverColor(std::move(customHoverColor)) {
		applyIconStyles(icon);
	};

	void leaveEvent(QEvent *p) override {
		setStyleSheet(IToolsNamespace::Styles::toolBarBackgroundColor);
	}

	void enterEvent(QEnterEvent *event) override {
		if (customHoverColor != nullptr) {
			setStyleSheet(
					IToolsNamespace::Styles::toolBarBackgroundColor +
					"background-color: " + customHoverColor
			);
		} else {
			setStyleSheet(IToolsNamespace::Styles::toolBarHoverColor);
		}
	}

	~ToolButton() override = default;
};

#endif //IT_TOOLS_TOOL_BUTTON_H
