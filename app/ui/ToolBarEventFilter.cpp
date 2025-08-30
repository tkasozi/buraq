//
// Created by talik on 2/7/2024.
//

#include <QEvent>
#include <QMouseEvent>

#include "ToolBarEventFilter.h"

#include "Config.h"
#include "app_ui/AppUi.h"
#include "ToolBar.h"

bool ToolBarEventFilter::eventFilter(QObject* obj, QEvent* e)
{
    const auto toolBar = dynamic_cast<ToolBar*>(obj);
    const auto event = dynamic_cast<QMouseEvent*>(e);

    if (toolBar == nullptr || event == nullptr)
    {
        return false; // Pass event to the object's normal handlers
    }

    auto* ui = dynamic_cast<AppUi*>(obj->parent());
    auto geo = toolBar->geometry();

    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
        // m_toolBar->updateMaxAndRestoreIconButton();

        return true;
    case QEvent::MouseButtonPress:

        if (event->button() == Qt::LeftButton)
        {
            isMouseBtnPressed = true;
            this->dragStartPos = event->pos();

            return true;
        }

        return false;
    case QEvent::MouseMove:
        if (isMouseBtnPressed && ui != nullptr)
        {
            if (ui->width() != Config::singleton().getWindow()->normalSize)
            {
                // m_toolBar->updateMaxAndRestoreIconButton();
            }

            const auto pos = event->globalPosition().toPoint();

            const QPoint newPos = toolBar->pos() + pos - dragStartPos;

            ui->setGeometry(newPos.x(), newPos.y(), Config::singleton().getWindow()->normalSize, geo.height());

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

ToolBarEventFilter::ToolBarEventFilter(QObject* parent)
    : QObject(parent),
      dragStartPos(0, 0),
      isMouseBtnPressed(false)
{
}

ToolBarEventFilter::~ToolBarEventFilter() = default;
