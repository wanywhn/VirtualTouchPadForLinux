#include "touchscreen.h"

#include <QTouchEvent>

TouchScreen::TouchScreen(QWidget *parent) : QWidget(parent)
{

}

bool TouchScreen::event(QEvent *event)
{
    switch(event->type()) {
    case QEvent::TouchBegin:
        this->tp.clearState();
        auto tevent = dynamic_cast<QTouchEvent *>(event);
        auto touchPoints = tevent->touchPoints();
        for (auto point : touchPoints) {
            this->tp.addPoint(point.id(), {point.pos().x(),point.pos().y()});
        }
        this->tp.SendStatusPacket();
        this->tp.SendHeadPacket();

        break;
    case QEvent::TouchUpdate:
        break;
    case QEvent::TouchEnd:
        break;
    }
}
