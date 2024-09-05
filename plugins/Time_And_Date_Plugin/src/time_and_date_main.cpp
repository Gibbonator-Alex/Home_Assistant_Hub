#include "include/time_and_date_main.h"
#include "../ui/ui_time_and_date_main.h"

Time_And_Date_Main::Time_And_Date_Main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Time_And_Date_Main)
{
    ui->setupUi(this);
}

Time_And_Date_Main::~Time_And_Date_Main()
{
    delete ui;
}
