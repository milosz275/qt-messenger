#include "qt_messenger.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_messenger w;
    w.show();
    return a.exec();
}
