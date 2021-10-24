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
    void backToStartScreen();


    // QObject interface
public:
    bool event(QEvent *event) override;

public:
    TouchPad tp;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TOUCHSCREEN_H
