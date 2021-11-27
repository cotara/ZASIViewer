#include "looker.h"
#include "ui_looker.h"

Looker::Looker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Looker)
{
    ui->setupUi(this);
    diamLooker1 = new diameterLooker(this, 1);
    diamLooker2 = new diameterLooker(this, 2);
    ui->HLayout->addWidget(diamLooker1);
    ui->HLayout->addWidget(diamLooker2);
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

void Looker::setNumDev1(int num){
    numDev1 = num;
}

void Looker::setNumDev2(int num){
    numDev2 = num;
}
