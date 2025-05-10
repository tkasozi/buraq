//
// Created by talik on 2/16/2024.
//

#ifndef IT_TOOLS_ICON_BUTTON_H
#define IT_TOOLS_ICON_BUTTON_H

#include <QString>
#include <QWidget>

#include "ToolButton.h"

class IconButton : public ToolButton {

public:
	explicit IconButton(const QString &fileName, const int w = 32, int h = 32, const QString &customStyles = "")
			: ToolButton(fileName) {
		setFixedSize(w, h);
		setStyleSheet(customStyles + "border: 0px;");
		this->customStyles = customStyles;
	}

	void leaveEvent(QEvent *p) override {
		setStyleSheet(
				"border: 0px;" +
				IToolsNamespace::Styles::commonToolBarStyles
		);
	}

	void enterEvent(QEnterEvent *event) override {
		if (customStyles != "") {
			setStyleSheet(
					"border-radius: 10px;" +
					customStyles
			);
		} else {
			setStyleSheet(
					"border-radius: 10px;" +
					IToolsNamespace::Styles::toolBarHoverColor
			);
		}
	}

	~IconButton() override = default;

private:
	QString customStyles;
};

#endif //IT_TOOLS_ICON_BUTTON_H
