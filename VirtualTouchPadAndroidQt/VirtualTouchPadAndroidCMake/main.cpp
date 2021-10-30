#include <QApplication>
#include "startscreen.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents,true);
    QApplication a(argc, argv);
    StartScreen w;
    w.show();

    return QApplication::exec();
}
