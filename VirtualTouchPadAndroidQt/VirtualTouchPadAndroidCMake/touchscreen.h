#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "PacketBuilder/PacketConfigBuilder.h"
#include "touchPad.h"
#include "serverconfig.h"

#include <QSettings>
#include <QWidget>
#include <QApplication>
#include <QScreen>

class TouchScreen : public QWidget {
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

public slots:

    void connectToServer(void) {
        QSettings settings(this);

        auto connected = this->tp.connectTo(settings.value(SETTINGS_IP).toString().toStdString().c_str(),
                                            settings.value(SETTINGS_PORT).toInt());
        assert(this->tp.isConnected() == true);
        auto builder = new PacketConfigBuilder();
        builder->setConnect(true);
        builder->setResX(settings.value(SETTINGS_RESOLUTION).toInt());
        builder->setResY(settings.value(SETTINGS_RESOLUTION).toInt());
        builder->setMaxX(qApp->screens()[0]->size().width());
        builder->setMaxY(qApp->screens()[0]->size().height());
        this->tp.sendData(builder->getBytes(), 6);
        delete builder;
    }

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // TOUCHSCREEN_H
