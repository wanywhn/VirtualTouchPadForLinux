#include <QApplication>
#include "startscreen.h"

int main(int argc, char *argv[]) {
    QApplication::setOrganizationName(APP_ORGANIZATION_NAME);
    QApplication::setOrganizationDomain(APP_ORGANIZATION_DOMAIN);
    QApplication::setApplicationName(APP_APPLICATION_NAME);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, true);
    QApplication a(argc, argv);
    StartScreen w;
    w.show();

    return QApplication::exec();
}
