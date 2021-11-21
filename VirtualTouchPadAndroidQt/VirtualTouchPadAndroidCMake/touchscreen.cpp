#include "touchscreen.h"
#include <QDebug>

#include <QTouchEvent>

TouchScreen::TouchScreen(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_AcceptTouchEvents);
}

bool TouchScreen::event(QEvent *event) {
    auto tevent = dynamic_cast<QTouchEvent *>(event);
    if (tevent == nullptr) return false;
    switch (event->type()) {
        case QEvent::MouseMove: {
        }
        case QEvent::TouchBegin: {
            qDebug() << QEvent::TouchBegin;
            qDebug() << tevent->touchPoints().size();
/*
            this->tp.clearState();
            auto touchPoints = tevent->touchPoints();

            for (const auto &point: touchPoints) {
                this->tp.addPoint(point.id(), {(int) point.pos().x(), (int) point.pos().y()});
            }
            this->tp.SendStatusPacket();
            this->tp.SendHeadPacket();
            return true;
            */

        }
        case QEvent::TouchUpdate: {
            // 当多个触点非同时检测到按下时,似乎不存在该场景，每次均会进入TouchBegin
            auto count = tevent->touchPoints().count();
            if (count == 0) {
                break;
            }
            for (const auto &point: tevent->touchPoints()) {
                switch (point.state()) {
                    case Qt::TouchPointMoved:
                    case Qt::TouchPointPressed: {
                        this->tp.addPoint(point.id(),
                                          {static_cast<int>(point.pos().x()), static_cast<int>(point.pos().y())});
                        break;
                    }
                    case Qt::TouchPointReleased: {
                        this->tp.removePoint(point.id());

                        break;
                    }
                    default:
                        break;

                }
            }
            if (this->tp.mLastFingerCount != this->tp.PointCounts()) {
                qDebug() << "finger num changed, prev[%d]" << this->tp.mLastFingerCount << " curr[%d]"
                         << this->tp.PointCounts();
                this->tp.SendStatusPacket();
                this->tp.SendHeadPacket();
                this->tp.mLastFingerCount = this->tp.PointCounts();
            }

            if (count == 1) {
//                qDebug()<<"one finger";
                this->tp.SendHeadPacketForOneFingerWhenMotion();
            } else {
                this->tp.updatePointMotion();
            }

            return true;

        }
        case QEvent::TouchEnd: {
//            for (auto const &item :tevent->touchPoints()) {
            qDebug() << QEvent::TouchEnd;
            qDebug() << tevent->touchPoints().size();
//                qDebug()<<item.id();
//                qDebug() << item;
//                this->tp.removePoint(item.id());
//            }
            this->tp.clearState();

            this->tp.SendStatusPacket();
            this->tp.SendHeadPacket();
            return true;

        }
        default:
            break;
    }
    return false;
}

void TouchScreen::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Back) {
        emit backToStartScreen();
        event->accept();
    }
}
