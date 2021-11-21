#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QWidget>

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
