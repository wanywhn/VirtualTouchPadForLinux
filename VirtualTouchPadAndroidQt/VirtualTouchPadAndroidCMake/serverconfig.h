#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QWidget>
#include <QKeyEvent>

#define APP_ORGANIZATION_NAME "wanywhn"
#define APP_ORGANIZATION_DOMAIN "gitee.com.wanywhn"
#define APP_APPLICATION_NAME "VirtualTouchPadClient"
#define SETTINGS_IP "connect/ip"
#define SETTINGS_PORT "connect/port"
#define SETTINGS_RESOLUTION "touchscreen/resolution"

class ServerConfig : public QWidget {
Q_OBJECT
public:
    explicit ServerConfig(QWidget *parent = nullptr);

signals:

    void backtoStartScreen();

protected:
    void keyPressEvent(QKeyEvent *event) override;



};

#endif // SERVERCONFIG_H
