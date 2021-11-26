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
void Looker::setData(double number, int dataType, int numDev){
    if(numDev == numDev1){//Если пришли данные первого девайса
        switch (dataType){
            case 0:
              diamLooker1->setDataD(number); break;
            case 1:
              diamLooker1->setDataDX(number); break;
            case 2:
              diamLooker1->setDataDY(number); break;
        }
    }
    else if(numDev == numDev2){//Если пришли данные второго девайса
        switch (dataType){
            case 0:
              diamLooker2->setDataD(number); break;
            case 1:
              diamLooker2->setDataDX(number); break;
            case 2:
              diamLooker2->setDataDY(number); break;
        }
    }
}

void Looker::setNumDev1(int num){
    numDev1 = num;
}

void Looker::setNumDev2(int num){
    numDev2 = num;
}
