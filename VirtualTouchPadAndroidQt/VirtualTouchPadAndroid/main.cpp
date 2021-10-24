//#include "startscreen.h"
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    StartScreen w;
    QWidget w;
    w.show();


    return a.exec();
}
