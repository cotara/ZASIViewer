#include "zasilooker.h"
#include "ui_zasilooker.h"
#include <QDebug>

ZasiLooker::ZasiLooker(QWidget *parent,int diam, int num) : Looker(parent,num),  ui(new Ui::ZasiLooker), m_model(diam)
{
    ui->setupUi(this);
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::black);
    ui->lcdNumber->setPalette(palette);
    ui->lcdNumber_2->setPalette(palette);
    ui->lcdNumber_3->setPalette(palette);

    fname = "Fault log "+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".xlsx";
}

ZasiLooker::~ZasiLooker()
{
    delete ui;
}


void ZasiLooker::rePaint()
{
    QVector <double> m_data = data();
    m_model = m_data.at(0);
    m_vers = m_data.at(1);
    m_ustMin = m_data.at(2);
    m_ustMax = m_data.at(3);
//    m_isHigh = m_data.at(10);
//    m_highCur = m_data.at(11);
//    m_ustCur = m_data.at(12);
//    m_voltageCur = m_data.at(13);
//    m_countBang = m_data.at(16);

    ui->lcdNumber->display(m_ustCur/100);//Текущая уставка по напряжению
    ui->lcdNumber_2->display(m_voltageCur/100);//Текущее амплитудное напряжение
    ui->lcdNumber_3->display(m_countBang/100);//Количество пробоев

    if(m_isHigh){
        ui->isHighLabel->setStyleSheet(red);
        ui->isHighLabel->setText("Высокое напряжение включено");
    }
    else {
        ui->isHighLabel->setStyleSheet(lightgreen);
        ui->isHighLabel->setText("Высокое напряжение отключено");
    }

}



void ZasiLooker::on_setUstButton_clicked(){
    QVector<unsigned short> data;
    data.append(ui->setUstBox->value());
    emit onSetReg(3, 1, data);
}

void ZasiLooker::on_onHighButton_clicked(bool checked){
    QVector<unsigned short> data;
    data.append(ui->onHighButton->isChecked());
    emit onSetReg(10, 1, data);

    if(checked)
        ui->onHighButton->setText("Отключить");
    else
        ui->onHighButton->setText("Включить");
}

void ZasiLooker::writeToExcel()
{
    //записать новую запись в ексель
    QXlsx::Document xlsx(fname);

       qDebug()<<xlsx.read("B5");

       QVariant temp = xlsx.read("B5");


       m_current_xlsx_line =  temp.toInt();

       QString fault_time = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");

       QXlsx::Format font_bold;
        font_bold.setFontBold(true);

        xlsx.write(m_current_xlsx_line+8,1,"Окончание сессии "+fault_time,font_bold);



        xlsx.save();
}

void ZasiLooker::on_DropDefectCountButton_clicked(){
    QVector<unsigned short> data;
    data.append(0);
    emit onSetReg(16, 1, data);
}




