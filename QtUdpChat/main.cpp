#include "QtUdpChat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtUdpChat w;
    w.show();
    return a.exec();
}
