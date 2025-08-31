//
// Created by talik on 2/7/2024.
//

#include <QEvent>
#include <QMouseEvent>

#include "ToolBarEvent.h"

#include "Config.h"
#include "../../app_ui/AppUi.h"
#include "../../ToolBar.h"
#include "frameless_window/FramelessWindow.h"

bool ToolBarEvent::eventFilter(QObject* obj, QEvent* event)
{
    const auto toolBar = dynamic_cast<ToolBar*>(obj);
    const auto _event = dynamic_cast<QMouseEvent*>(event);

    if (toolBar == nullptr || event == nullptr)
    {
        return false; // Pass event to the object's normal handlers
    }

    auto* ui = dynamic_cast<FramelessWindow*>(obj->parent());
    const auto geo = toolBar->geometry();

    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
        // m_toolBar->updateMaxAndRestoreIconButton();

        return true;
    case QEvent::MouseButtonPress:

        if (_event->button() == Qt::LeftButton)
        {
            isMouseBtnPressed = true;
            this->dragStartPos = _event->pos();

            return true;
        }

        return false;
    case QEvent::MouseMove:
        if (isMouseBtnPressed && ui != nullptr)
        {
            if (ui->width() != Config::singleton().getWindow()->normalSize)
            {
                // toolBar->updateMaxAndRestoreIconButton();
            }

            const auto pos = _event->globalPosition().toPoint();

            const QPoint newPos = toolBar->pos() + pos - dragStartPos;

            ui->setGeometry(newPos.x(), newPos.y(), Config::singleton().getWindow()->normalSize, geo.height());

            return true;
        }

        return false; // Stop event propagation (optional)
    case QEvent::MouseButtonRelease:

        isMouseBtnPressed = false;

        return true;
    default:
        return QObject::eventFilter(obj, event);
    }
}

ToolBarEvent::ToolBarEvent(QObject* parent)
    : QObject(parent),
      dragStartPos(0, 0),
      isMouseBtnPressed(false)
{
}

ToolBarEvent::~ToolBarEvent() = default;
