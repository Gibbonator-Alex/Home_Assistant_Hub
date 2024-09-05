#include "include/time_and_date_main.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Time_And_Date_Main w;
    w.show();
    return a.exec();
}
