#include "serverconfig.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <qnamespace.h>


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

    auto connectBtn = new QPushButton("Save Configure", this);
    connect(connectBtn, &QPushButton::clicked, [this, ipLineEdit, portLineEdit]() {
        if (ipLineEdit->text().isEmpty() || portLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Para Error", "please enter");
            return;
        }
        QSettings setting;
        setting.setValue(SETTINGS_IP, ipLineEdit->text().toStdString().data());
        setting.setValue(SETTINGS_PORT,portLineEdit->text().toStdString().data());
        QMessageBox msgBox;
        msgBox.setText("Save configure success");
        msgBox.exec();
    });
    layout->addRow(connectBtn);
}


void ServerConfig::keyPressEvent(QKeyEvent *event) {
        if(event->key() == Qt::Key_Back) {
                emit backtoStartScreen();
        }
}
