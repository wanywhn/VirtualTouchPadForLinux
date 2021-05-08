#include "startscreen.h"
#include "touchscreen.h"
#include "ui_startscreen.h"

#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>

StartScreen::StartScreen(QWidget *parent) :
    QStackedWidget(parent)
{
    this->setupUI();
}

void StartScreen::setupUI()
{
    this->mainWidget = new QWidget(this);
    this->touchScreenWidget = new TouchScreen(this);

    auto verticalLayout = new QVBoxLayout(this->mainWidget);


    auto setupConnBtn = new QPushButton("Setup Connection");
    verticalLayout->addWidget(setupConnBtn);

    auto touchScreenBtn = new QPushButton("Touch Screen");
    connect(touchScreenBtn, &QPushButton::clicked, [this](){
        this->setCurrentWidget(this->touchScreenWidget);
    });
    verticalLayout->addWidget(touchScreenBtn);

    auto testConnBtn = new QPushButton("Test Connect");
    verticalLayout->addWidget(testConnBtn);


    this->addWidget(this->mainWidget);
    this->addWidget(this->touchScreenWidget);
    this->setCurrentWidget(this->mainWidget);
}

StartScreen::~StartScreen()
{
}


void StartScreen::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Back) {
        this->setCurrentWidget(this->mainWidget);
        event->accept();
    }
}
