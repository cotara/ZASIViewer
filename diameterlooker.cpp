#include "diameterlooker.h"
#include "ui_diameterlooker.h"

diameterLooker::diameterLooker(QWidget *parent, int num) :  QGroupBox(parent), ui(new Ui::diameterLooker)
{
    ui->setupUi(this);
    setTitle("Прибор №" + QString::number(num));
}

diameterLooker::~diameterLooker()
{
    delete ui;

}

void diameterLooker::setDataD(double data)
{
    d=data;
    ui->lcdNumber->display(d);
}

void diameterLooker::setDataDX(double data)
{
    dx=data;
    ui->lcdNumber_2->display(dx);
}

void diameterLooker::setDataDY(double data)
{
    dy=data;
    ui->lcdNumber_3->display(dy);
}
