#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QKeyEvent>
#include <QStackedWidget>
#include "serverconfig.h"
#include "touchscreen.h"

class StartScreen : public QStackedWidget {
Q_OBJECT
public:
    explicit StartScreen(QWidget *parent = nullptr);

    void setupUI();

    ~StartScreen() override = default;

    QWidget *mainWidget;
    ServerConfig *setupConnWidget;
    TouchScreen *touchScreenWidget;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // STARTSCREEN_H
