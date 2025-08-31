//
// Created by talik on 2/7/2024.
//

#ifndef IT_TOOLS_WINDOW_EVENT_FILTER_H
#define IT_TOOLS_WINDOW_EVENT_FILTER_H

#include <QMainWindow>
#include <QMetaObject>
#include <iostream>

class ToolBarEvent final : public QObject {
	Q_OBJECT

public:
	explicit ToolBarEvent(QObject *parent = nullptr);
	~ToolBarEvent() override;
	bool eventFilter(QObject *obj, QEvent *event) override;

private:
	QPoint dragStartPos;
	bool isMouseBtnPressed;
};


#endif //IT_TOOLS_WINDOW_EVENT_FILTER_H
