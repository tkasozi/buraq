//
// Created by talik on 3/12/2024.
//

#ifndef IT_TOOLS_FILEPATHLABEL_H
#define IT_TOOLS_FILEPATHLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <utility>
#include "CustomDrawer.h"
#include "CustomLabel.h"

class FilePathLabel : public CustomLabel {
Q_OBJECT

public slots:

	void activeLabel() {
		isActive = true;
		setStyles();
	}

	void reset() {
		isActive = false;
		// remove highlight css
		setStyleSheet(commonCss);
	}

public:
	explicit FilePathLabel(QString filePath, QWidget *parent = nullptr)
			: CustomLabel(parent), isActive(false), filePath(std::move(filePath)) {
		setMouseTracking(true);
		setStyleSheet(
				commonCss
		);
	}
	~FilePathLabel() override = default;

	QString getFilePath() {
		return filePath;
	};

protected:
	void leaveEvent(QEvent *p) override {
		if (!isActive) {
			setStyleSheet(
					commonCss
			);
		}
	}

	void enterEvent(QEnterEvent *event) override {
		setStyles();
	}

private:
	void setStyles() {
		if (hasMouseTracking()) {
			setStyleSheet(
					commonCss +
					"color: #C2C2C2;"
					"background-color: #383838;"
			);
		}
	}

	QString commonCss = "background-color: none;"
						"padding: 4px;";
	bool isActive;
	QString filePath;
};

#endif //IT_TOOLS_FILEPATHLABEL_H
