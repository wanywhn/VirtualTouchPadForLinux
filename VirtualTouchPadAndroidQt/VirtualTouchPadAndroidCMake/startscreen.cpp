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
    this->mainWidget = new QWidget(this);
    this->touchScreenWidget = new TouchScreen(this);
    this->setupConnWidget = new ServerConfig(this);

    auto mainLayout = new QVBoxLayout(this);

    auto verticalLayout = new QVBoxLayout(this->mainWidget);

    auto setupConnBtn = new QPushButton("Setup Connection");
    verticalLayout->addWidget(setupConnBtn);

    auto touchScreenBtn = new QPushButton("Touch Screen");
    touchScreenBtn->setEnabled(false);
    verticalLayout->addWidget(touchScreenBtn);

    connect((TouchScreen *) this->touchScreenWidget, &TouchScreen::backToStartScreen, [this]() {
        this->setCurrentWidget(this->mainWidget);
    });
    connect(setupConnBtn, &QPushButton::clicked, [this]() {
        this->setCurrentWidget(this->setupConnWidget);
    });

    connect(touchScreenBtn, &QPushButton::clicked, [this]() {
        this->setCurrentWidget(this->touchScreenWidget);
    });
    connect(this->setupConnWidget, &ServerConfig::connectToServer, this->touchScreenWidget,
            &TouchScreen::connectToServer);
    connect(this->setupConnWidget, &ServerConfig::backtoStartScreen, [this]() {
        this->setCurrentWidget(this->mainWidget);
    });

    connect(this->touchScreenWidget, &TouchScreen::serverConnectStatus, [this, touchScreenBtn](bool connected) {
        if (!connected) {
            touchScreenBtn->setEnabled(false);
            return;
        }
        touchScreenBtn->setEnabled(true);
        auto builder = new PacketConfigBuilder();
        builder->setConnect(true);
        builder->setResX(10);
        builder->setResY(10);
        builder->setMaxX(qApp->screens()[0]->size().width());
        builder->setMaxY(qApp->screens()[0]->size().height());
        ((TouchScreen *) this->touchScreenWidget)->tp.sendData(builder->getBytes(), 6);
        delete builder;
    });
    connect(this->touchScreenWidget, &TouchScreen::serverConnectStatus, this->setupConnWidget,
            &ServerConfig::connectStatus);

    this->addWidget(this->mainWidget);
    this->addWidget(this->touchScreenWidget);
    this->addWidget(this->setupConnWidget);
    this->setCurrentWidget(this->mainWidget);

}


void StartScreen::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Back) {
        this->setCurrentWidget(this->mainWidget);
    }
}

