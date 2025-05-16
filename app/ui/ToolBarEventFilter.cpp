//
// Created by talik on 2/7/2024.
//

#include <QEvent>
#include <QMouseEvent>

#include "ToolBarEventFilter.h"
#include "IToolsUi.h"
#include "ToolBar.h"
#include "../utils/Utils.h"


bool ToolBarEventFilter::eventFilter(QObject *obj, QEvent *e) {

	auto *toolBar = dynamic_cast<ToolBar *>(obj);
	auto *event = dynamic_cast<QMouseEvent *>(e);

	if (toolBar == nullptr || event == nullptr) {
		return false; // Pass event to the object's normal handlers
	}

	auto *ui = dynamic_cast<IToolsUi *>(obj->parent());
	auto geo = toolBar->geometry();

	switch (event->type()) {
		case QEvent::MouseButtonDblClick:
			toolBar->updateMaxAndRestoreIconButton();

			return true;
		case QEvent::MouseButtonPress:

			if (event->button() == Qt::LeftButton) {

				isMouseBtnPressed = true;
				this->dragStartPos = event->pos();

				return true;
			}

			return false;
		case QEvent::MouseMove:
			if (isMouseBtnPressed) {
				if (ui->width() != ItoolsNS::main_config.getWindow().normalSize) {
					toolBar->updateMaxAndRestoreIconButton();
				}

				auto pos = event->globalPosition().toPoint();

				QPoint newPos = toolBar->pos() + pos - dragStartPos;

				ui->setGeometry(newPos.x(), newPos.y(), ItoolsNS::main_config.getWindow().normalSize, geo.height());

				return true;
			}

			return false; // Stop event propagation (optional)
		case QEvent::MouseButtonRelease:

			isMouseBtnPressed = false;

			return true;
		default:
			return QObject::eventFilter(obj, e);
	}
}

ToolBarEventFilter::ToolBarEventFilter()
		: dragStartPos(0, 0),
		  isMouseBtnPressed(false) {
}

ToolBarEventFilter::~ToolBarEventFilter() = default;
