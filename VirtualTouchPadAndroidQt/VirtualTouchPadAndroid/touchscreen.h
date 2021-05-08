#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "touchPad.h"

#include <QWidget>

class TouchScreen : public QWidget
{
    Q_OBJECT
public:
    explicit TouchScreen(QWidget *parent = nullptr);

signals:


    // QObject interface
public:
    bool event(QEvent *event) override;

private:
    TouchPad tp;
};

#endif // TOUCHSCREEN_H
