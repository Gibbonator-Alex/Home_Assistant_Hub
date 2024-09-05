#ifndef TIME_AND_DATE_MAIN_H
#define TIME_AND_DATE_MAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class Time_And_Date_Main;
}
QT_END_NAMESPACE

class Time_And_Date_Main : public QMainWindow
{
    Q_OBJECT

public:
    Time_And_Date_Main(QWidget *parent = nullptr);
    ~Time_And_Date_Main();

private:
    Ui::Time_And_Date_Main *ui;
};
#endif // TIME_AND_DATE_MAIN_H
