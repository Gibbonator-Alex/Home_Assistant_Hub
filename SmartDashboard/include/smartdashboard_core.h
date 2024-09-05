#ifndef SMARTDASHBOARD_CORE_H
#define SMARTDASHBOARD_CORE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class SmartDashboard_Core; }
QT_END_NAMESPACE

class SmartDashboard_Core : public QMainWindow
{
    Q_OBJECT

public:
    SmartDashboard_Core(QWidget *parent = nullptr);
    ~SmartDashboard_Core();

private:
    Ui::SmartDashboard_Core *ui;
};
#endif // SMARTDASHBOARD_CORE_H
