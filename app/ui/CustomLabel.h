//
// Created by talik on 4/19/2024.
//

#ifndef IT_TOOLS_CUSTOMLABEL_H
#define IT_TOOLS_CUSTOMLABEL_H

#include <QMouseEvent>
#include <QLabel>

class CustomLabel : public QLabel {
Q_OBJECT

public:
	explicit CustomLabel(QWidget *parent = nullptr);

protected:
	bool event(QEvent *event) override;

	~CustomLabel() override = default;

signals:

	void clicked();

};

#endif //IT_TOOLS_CUSTOMLABEL_H
