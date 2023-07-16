#include "serverconfig.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QLabel>


ServerConfig::ServerConfig(QWidget *parent) : QWidget(parent) {
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &ServerConfig::addService);
    connect(&zeroConf, &QZeroConf::serviceRemoved, this, &ServerConfig::removeService);
    connect(&zeroConf, &QZeroConf::serviceUpdated, this, &ServerConfig::updateService);

    this->startPublish();
//    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(appStateChanged(Qt::ApplicationState)));


    auto layout = new QFormLayout(this);
    auto ipLineEdit = new QLineEdit();
    QSettings settings;
    ipLineEdit->setText(settings.value(SETTINGS_IP, "127.0.0.1").toString());
    layout->addRow("Server IP", ipLineEdit);
    auto portLineEdit = new QLineEdit();
    portLineEdit->setText(settings.value(SETTINGS_PORT, "6781").toString());
    layout->addRow("Server Port", portLineEdit);
    
    auto slider = new QSlider(this);
    slider->setTracking(false);
    auto sliderText = new QLabel(QString("Reso: %1(unit/mm)").arg(settings.value(SETTINGS_RESOLUTION).toInt()), this);
    layout->addRow(sliderText, slider);
    slider->setValue(settings.value(SETTINGS_RESOLUTION, 10).toInt());
    slider->setOrientation(Qt::Horizontal);
    connect(slider, &QSlider::sliderMoved, [sliderText](int value){
                    QSettings settings;
                    sliderText->setText(QString("Reso:%1").arg(value));
                    });
    connect(slider, &QSlider::valueChanged, [sliderText](int value){
                    QSettings settings;
                    settings.setValue(SETTINGS_RESOLUTION, value);
            });

    auto connectBtn = new QPushButton("Save Configure", this);
    connect(connectBtn, &QPushButton::clicked, [this, ipLineEdit, portLineEdit]() {
        if (ipLineEdit->text().isEmpty() || portLineEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Para Error", "please enter");
            return;
        }
        QSettings setting;
        setting.setValue(SETTINGS_IP, ipLineEdit->text());
        setting.setValue(SETTINGS_PORT,portLineEdit->text());
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

void ServerConfig::startPublish()
{
        zeroConf.clearServiceTxtRecords();
        zeroConf.addServiceTxtRecord("Qt", "the best!");
        zeroConf.addServiceTxtRecord("ZeroConf is nice too");
        zeroConf.startServicePublish("Android Phone", "_virtualtouchpad._tcp", "local", 6781);
        qDebug() << "Start Publish";

}

// ---------- Discovery Callbacks ----------

void ServerConfig::addService(QZeroConfService zcs)
{
        qDebug() << "Added service: " << zcs;


}

void ServerConfig::removeService(QZeroConfService zcs)
{
        qint32 i, row;
        qDebug() << "Removed service: " << zcs;
}

void ServerConfig::updateService(QZeroConfService zcs)
{
        qDebug() << "Service updated: " << zcs;
}


void ServerConfig::appStateChanged(Qt::ApplicationState state)
{
        if (state == Qt::ApplicationSuspended) {
                zeroConf.stopServicePublish();
                zeroConf.stopBrowser();
        }
        else if (state == Qt::ApplicationActive) {
//                if (publishEnabled && !zeroConf.publishExists())
//                    startPublish();
                if (!zeroConf.browserExists())
                    zeroConf.startBrowser("_qtzeroconf_test._tcp");
        }
}
