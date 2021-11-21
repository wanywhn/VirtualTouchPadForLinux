#include "serverconfig.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>


#define SETTINGS_IP "connect/ip"
#define SETTINGS_PORT "connect/port"

ServerConfig::ServerConfig(QWidget *parent) : QWidget(parent) {
    auto layout = new QFormLayout(this);
    auto ipLineEdit = new QLineEdit();
    QSettings settings;
    ipLineEdit->setText(settings.value(SETTINGS_IP, "127.0.0.1").toString());
    layout->addRow("Server IP", ipLineEdit);
    auto portLineEdit = new QLineEdit();
    portLineEdit->setText(settings.value(SETTINGS_PORT, "6781").toString());
    layout->addRow("Server Port", portLineEdit);

    auto connectBtn = new QPushButton("Connect", this);
    connect(connectBtn, &QPushButton::clicked, [this, ipLineEdit, portLineEdit]() {
        if (ipLineEdit->text().isEmpty() || portLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Para Error", "please enter");
            return;
        }
        QSettings setting;
        setting.setValue(SETTINGS_IP, ipLineEdit->text().toStdString().data());
        setting.setValue(SETTINGS_PORT,portLineEdit->text().toStdString().data());
        emit connectToServer(ipLineEdit->text().toStdString().data(), portLineEdit->text().toInt());
    });
    layout->addRow(connectBtn);

}

void ServerConfig::connectStatus(bool connected) {
    QMessageBox msgBox;
    if (connected) {
        msgBox.setText("Connect to Server Success");
        msgBox.exec();
        emit backtoStartScreen();
    } else {
        msgBox.setText("Connect to Server Fail");
        msgBox.exec();
    }
}
