#include "serverconfig.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QLabel>



ServerConfig::ServerConfig(QWidget *parent) : QWidget(parent) {
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
