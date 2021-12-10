#include "zasilooker.h"
#include "ui_zasilooker.h"
#include <QDebug>
#include <QFileDialog>

ZasiLooker::ZasiLooker(QWidget *parent,int diam, int num) : Looker(parent,num),  ui(new Ui::ZasiLooker), m_model(diam){
    ui->setupUi(this);
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::black);
    ui->lcdNumber->setPalette(palette);
    ui->lcdNumber_2->setPalette(palette);
    ui->lcdNumber_3->setPalette(palette);
    m_excelFile = new ExcelWriter(this);

    ui->isHighLabel->setObjectName("BigLabel");//css t
    setEnabled(false);
}

ZasiLooker::~ZasiLooker(){
    delete ui;
}

void ZasiLooker::rePaint(){
    QVector <short> m_data = data();
    m_model = m_data.at(0);
    m_vers = m_data.at(1);
    m_ustMin = m_data.at(2);
    m_ustMax = m_data.at(3);

    //Если это первое  сообщение сессии
    if(m_isHigh==-1){
        emit modelChanged(m_model);
        m_excelFile->writeStartMessage(QString::number(m_model));
        m_excelFile->writeToFile(highInstarting,m_data.at(10));
    }
    //Если значение изменилось
    else if(m_isHigh!=m_data.at(10))
        m_excelFile->writeToFile(highChanged,m_data.at(10));
    m_isHigh = m_data.at(10);

    m_highCur = m_data.at(11);

    if(m_ustCur==-1)
         m_excelFile->writeToFile(ustInstarting,m_data.at(12));
    else if(m_ustCur!=m_data.at(12))
        m_excelFile->writeToFile(ustChanged,m_data.at(12));
    m_ustCur = m_data.at(12);

    m_voltageCur = m_data.at(14);

    if(m_countBang==-1)
        m_excelFile->writeToFile(countBangsInstarting,m_data.at(16));
    else if(m_countBang!=m_data.at(16))
        m_excelFile->writeToFile(countBangsChanged,m_data.at(16));
    m_countBang = m_data.at(16);


    ui->lcdNumber->display(m_ustCur/100.0);//Текущая уставка по напряжению
    ui->lcdNumber_2->display(m_voltageCur/100.0);//Текущее амплитудное напряжение
    ui->lcdNumber_3->display(m_countBang);//Количество пробоев

    if(m_isHigh){
        ui->isHighLabel->setStyleSheet(red);
        ui->isHighLabel->setText("Высокое напряжение включено");  
        ui->onHighButton->setChecked(true);
        ui->onHighButton->setText("Отключить");
    }
    else {
        ui->isHighLabel->setStyleSheet(gray);
        ui->isHighLabel->setText("Высокое напряжение отключено");
    }  
}

void ZasiLooker::onConnect(bool state){
    if(state){
        m_excelFile = new ExcelWriter;
    }
    else{
        QString path = QFileDialog::getExistingDirectory(this, "Выберите папку для сохранения", "./log");
        m_excelFile->close(path+"\\ZASI log "+ QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".xlsx");
        delete m_excelFile;
        m_excelFile=nullptr;
        m_isHigh=-1;
        m_model=-1;
        m_vers=-1;
        m_ustMin=-1;
        m_ustMax=-1;
        m_ustCur=-1;
        m_highCur=-1;
        m_voltageCur=-1;
        m_countBang=-1;
    }
}

void ZasiLooker::on_setUstButton_clicked(){
    QVector<unsigned short> data;
    data.append(static_cast<unsigned short>(ui->setUstBox->value()*100));
    emit onSetReg(12, 1, data);
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

void ZasiLooker::on_DropDefectCountButton_clicked(){
    QVector<unsigned short> data;
    data.append(0);
    emit onSetReg(16, 1, data);
    m_countBang=0;
    if(m_excelFile)
        m_excelFile->writeToFile(dropDeffectCount,0);
}
