#include "startscreen.h"
#include "ui_startscreen.h"
#include "touchscreen.h"

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QScreen>

#include <PacketBuilder/PacketConfigBuilder.h>

StartScreen::StartScreen(QWidget *parent) :
        QStackedWidget(parent) {
    this->setupUI();
}

void StartScreen::setupUI() {
    QSettings settings;
    this->mainWidget = new QWidget(this);
    this->touchScreenWidget = new TouchScreen(this);
    this->setupConnWidget = new ServerConfig(this);

    auto mainLayout = new QVBoxLayout(this);

    auto verticalLayout = new QVBoxLayout(this->mainWidget);

    auto setupConnBtn = new QPushButton("Setup Connection");
    verticalLayout->addWidget(setupConnBtn);

    auto touchScreenBtn = new QPushButton("Touch Screen");
    if (settings.contains(SETTINGS_IP)) {
            touchScreenBtn->setEnabled(true);
    } else {
            touchScreenBtn->setEnabled(false);
    }
    touchScreenBtn->setText(QString("Connect to [%1:%2], reso[%3]").arg(settings.value(SETTINGS_IP).toString()).arg(settings.value(SETTINGS_PORT).toInt()).arg(settings.value(SETTINGS_RESOLUTION).toInt()));
    verticalLayout->addWidget(touchScreenBtn);

    connect((TouchScreen *) this->touchScreenWidget, &TouchScreen::backToStartScreen, [this]() {
        this->setCurrentWidget(this->mainWidget);
    });
    connect(setupConnBtn, &QPushButton::clicked, [this]() {
        this->setCurrentWidget(this->setupConnWidget);
    });

    connect(touchScreenBtn, &QPushButton::clicked, [this]() {
        this->touchScreenWidget->connectToServer();

        this->setCurrentWidget(this->touchScreenWidget);
    });
    connect(this->setupConnWidget, &ServerConfig::backtoStartScreen, [this, touchScreenBtn]() {
                    QSettings settings;
    touchScreenBtn->setText(QString("Connect to [%1:%2], reso[%3]").arg(settings.value(SETTINGS_IP).toString()).arg(settings.value(SETTINGS_PORT).toInt()).arg(settings.value(SETTINGS_RESOLUTION).toInt()));
        this->setCurrentWidget(this->mainWidget);
    });

    this->addWidget(this->mainWidget);
    this->addWidget(this->touchScreenWidget);
    this->addWidget(this->setupConnWidget);
    this->setCurrentWidget(this->mainWidget);

}


void StartScreen::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Back) {
        this->touchScreenWidget->tp.disconn();
        this->setCurrentWidget(this->mainWidget);
    }
}

