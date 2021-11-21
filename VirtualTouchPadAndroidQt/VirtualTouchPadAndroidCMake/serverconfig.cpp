#include "serverconfig.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

ServerConfig::ServerConfig(QWidget *parent) : QWidget(parent) {
    auto layout = new QFormLayout(this);
    auto ipLineEdit = new QLineEdit();
    ipLineEdit->setText("127.0.0.1");
    layout->addRow("Server IP", ipLineEdit);
    auto portLineEdit = new QLineEdit();
    portLineEdit->setText("6781");
    layout->addRow("Server Port", portLineEdit);

    auto connectBtn = new QPushButton("Connect", this);
    connect(connectBtn, &QPushButton::clicked, [this, ipLineEdit, portLineEdit]() {
        if (ipLineEdit->text().isEmpty() || portLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Para Error", "please enter");
            return;
        }
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
