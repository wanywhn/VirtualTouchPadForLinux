#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "touchPad.h"
#include "serverconfig.h"

#include <QSettings>
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

    void connectToServer(void) {
        QSettings settings(this);

        auto connected = this->tp.connectTo(settings.value(SETTINGS_IP).toString().toStdString().c_str(),
                                            settings.value(SETTINGS_PORT).toInt());
        emit serverConnectStatus(connected);
    }

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TOUCHSCREEN_H
