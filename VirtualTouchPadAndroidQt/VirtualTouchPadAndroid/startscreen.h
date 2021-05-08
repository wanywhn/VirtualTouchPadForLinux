#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QKeyEvent>
#include <QStackedWidget>


class StartScreen : public QStackedWidget
{
    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    void setupUI(void);
    ~StartScreen();

    QWidget *mainWidget;
    QWidget *setupConnWidget;
    QWidget *touchScreenWidget;
    QWidget *testConnWidget;


    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // STARTSCREEN_H
