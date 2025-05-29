//
// Created by talik on 3/2/2024.
//

#ifndef EDITOR_MARGIN_H
#define EDITOR_MARGIN_H

#include <QWidget>
#include "Editor.h"
#include "CodeRunner.h"
#include "CommonWidget.h"

class EditorMargin : public CommonWidget {

public slots:
	void lineNumberAreaPaintEvent(QPaintEvent *event);

public:

	explicit EditorMargin(QWidget *appUi);

	[[nodiscard]] QSize sizeHint() const override {
		//return {((Editor *)editor)->lineNumberAreaWidth(), 0};
		return {0, 0};
	}

	~EditorMargin() override = default;

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	QWidget *appUi;
	std::unique_ptr<QWidget> line_numbers;
	std::unique_ptr<CodeRunner> codeRunner;

	void setupSignals() const override;
};

#endif //EDITOR_MARGIN_H
