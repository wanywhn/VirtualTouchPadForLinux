#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QWidget>

#define APP_ORGANIZATION_NAME "wanywhn"
#define APP_ORGANIZATION_DOMAIN "gitee.com.wanywhn"
#define APP_APPLICATION_NAME "VirtualTouchPadClient"

class ServerConfig : public QWidget {
Q_OBJECT
public:
    explicit ServerConfig(QWidget *parent = nullptr);

public slots:

    void connectStatus(bool connected);

signals:

    void connectToServer(const char *serverAddr, int port);

    void backtoStartScreen();

public slots:
};

#endif // SERVERCONFIG_H
