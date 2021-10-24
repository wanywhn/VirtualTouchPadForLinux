#include "startscreen.h"
#include "ui_startscreen.h"
#include "touchscreen.h"

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

#include <PacketBuilder/PacketConfigBuilder.h>

StartScreen::StartScreen(QWidget *parent) :
    QStackedWidget(parent)
{
    this->setupUI();
}

void StartScreen::setupUI()
{
    this->mainWidget = new QWidget(this);
    this->touchScreenWidget = new TouchScreen(this);

    this->touchScreenWidget->tp.connectTo("127.0.0.1", 6781);
    auto builder = new PacketConfigBuilder();
    builder->setConnect(true);
    builder->setResX(10);
    builder->setResY(10);
    builder->setMaxX(1080);
    builder->setMaxY(1920);
    ((TouchScreen *) this->touchScreenWidget)->tp.sendData(builder->getBytes(), 6);
    connect((TouchScreen *)this->touchScreenWidget, &TouchScreen::backToStartScreen,[this](){
        this->setCurrentWidget(this->mainWidget);
    });

    auto verticalLayout = new QVBoxLayout(this->mainWidget);


    auto setupConnBtn = new QPushButton("Setup Connection");
    verticalLayout->addWidget(setupConnBtn);

    auto touchScreenBtn = new QPushButton("Touch Screen");
    connect(touchScreenBtn, &QPushButton::clicked, [this](){
        qDebug()<<":test";
        printf("tests\r\n");
        this->setCurrentWidget(this->touchScreenWidget);
    });
    verticalLayout->addWidget(touchScreenBtn);

    auto testConnBtn = new QPushButton("Test Connect");
    verticalLayout->addWidget(testConnBtn);


    this->addWidget(this->mainWidget);
    this->addWidget(this->touchScreenWidget);
    this->setCurrentWidget(this->mainWidget);
}


void StartScreen::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Back) {
        this->setCurrentWidget(this->mainWidget);
        event->accept();
    }
}
bool StartScreen::event(QEvent *event)
{
    return false;
}

