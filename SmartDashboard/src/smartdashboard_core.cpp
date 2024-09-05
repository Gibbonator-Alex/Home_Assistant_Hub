#include "include/smartdashboard_core.h"
#include "../ui/ui_smartdashboard_core.h"

SmartDashboard_Core::SmartDashboard_Core(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SmartDashboard_Core)
{
    ui->setupUi(this);
}

SmartDashboard_Core::~SmartDashboard_Core()
{
    delete ui;
}

