#include "looker.h"
#include "ui_looker.h"

Looker::Looker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Looker)
{
    ui->setupUi(this);
    diamLooker1 = new diameterLooker(this, 1,diam1);
    diamLooker2 = new diameterLooker(this, 2,diam2);
    ui->HLayout->addWidget(diamLooker1);
    ui->HLayout->addWidget(diamLooker2);
    diamLooker2->setVisible(false);
    diamLooker1->setEnabled(false);
    diamLooker2->setEnabled(false);
}

Looker::~Looker()
{
    delete ui;
    delete diamLooker1;
    delete diamLooker2;
}
//Устанавливает число number в LCD dataType(0-Основной диаметр, 1-Dx, 2-Dy) в отображалку под номером numDev
void Looker::setData(const QVector<double>& data, int numDev){
    if(numDev == numDev1){//Если пришли данные первого девайса
        diamLooker1->setData(data);
    }
    else if(numDev == numDev2){//Если пришли данные второго девайса
        diamLooker2->setData(data);;
    }
}

void Looker::setEnabled(int numDev, bool en)
{
    if(numDev == numDev1){
        diamLooker1->setEnabled(en);
    }
    else if(numDev == numDev2){
        diamLooker2->setEnabled(en);
    }
}

void Looker::setNumDev1(int num){
    numDev1 = num;
}

void Looker::setNumDev2(int num){
    numDev2 = num;
}

void Looker::setDiam1(int diam)
{
    diam1=diam;
    diamLooker1->setDiam(diam1);
}

void Looker::setDiam2(int diam)
{
    diam2=diam;
    diamLooker2->setDiam(diam2);
}

void Looker::enableSecondLooker(bool state){
    diamLooker2->setVisible(state);
}
