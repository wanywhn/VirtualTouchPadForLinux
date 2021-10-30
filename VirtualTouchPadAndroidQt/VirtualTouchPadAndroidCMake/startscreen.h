#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QKeyEvent>
#include <QStackedWidget>
#include "touchscreen.h"

class StartScreen : public QStackedWidget
{
    Q_OBJECT
public:
    explicit StartScreen(QWidget *parent = nullptr);
    void setupUI();
    ~StartScreen() override = default;

    QWidget *mainWidget{};
    QWidget *setupConnWidget{};
    TouchScreen *touchScreenWidget{};
    QWidget *testConnWidget{};

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif // STARTSCREEN_H
