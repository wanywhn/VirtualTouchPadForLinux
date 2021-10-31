#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "touchPad.h"

#include <QWidget>

class TouchScreen : public QWidget {
Q_OBJECT
public:
    explicit TouchScreen(QWidget *parent = nullptr);

signals:

    void backToStartScreen();
    void serverConnectStatus(bool connected);

    // QObject interface
public:
    bool event(QEvent *event) override;

public:
    TouchPad tp;

public slots:
    void connectToServer(const char* serverAddr, int port) {
        auto connected = this->tp.connectTo(serverAddr, port);
        emit serverConnectStatus(connected);
    }

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TOUCHSCREEN_H
