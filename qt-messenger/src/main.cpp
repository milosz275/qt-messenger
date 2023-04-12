#include "include/qt_messenger.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    msg::qt_messenger widget;
    widget.show();
    return app.exec();
}
