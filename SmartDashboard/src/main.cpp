#include "include/smartdashboard_core.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SmartDashboard_Core w;
    w.show();
    return a.exec();
}
