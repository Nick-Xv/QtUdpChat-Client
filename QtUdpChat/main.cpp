#include "QtUdpChat.h"
#include <QtWidgets/QApplication>
#include "Config.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	Config config;
    QtUdpChat w;
    w.show();
    return a.exec();
}
