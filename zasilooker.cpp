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
    m_timer = new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,&ZasiLooker::handlerTimer);
    m_timer->setInterval(1000);
}

ZasiLooker::~ZasiLooker(){
    delete ui;
    delete m_timer;
}

void ZasiLooker::rePaint(){
    QVector <short> m_data = data();
    m_model = m_data.at(0);
    m_vers = m_data.at(1);
    m_ustMin = m_data.at(2);
    m_ustMax = m_data.at(3);
    m_voltageCur = m_data.at(14);

    ui->setUstBox->setMinimum(m_ustMin/100.0);
    ui->setUstBox->setMaximum(m_ustMax/100.0);

    m_isHigh = m_data.at(10);
    //Если первый раз получаем пакет за эту сессию
    if(!startSessionFlag){
        emit modelChanged(m_model);//Устанавливаем модель в панель управления
        if(loggingProperty){
            m_excelFile->writeStartMessage(QString::number(m_model));
            m_excelFile->writeToFile(highInstarting,m_data.at(11));
            m_excelFile->writeToFile(ustInstarting,m_data.at(12));
            m_excelFile->writeToFile(countBangsInstarting,m_data.at(16));
        }
        m_ustCur = m_data.at(12);
        startSessionFlag = true;
    }
    else{
        if(loggingProperty){
            //Если изменилось текущее высокое
            if(m_highCur!=m_data.at(11))
                 m_excelFile->writeToFile(highChanged,m_data.at(11));
            //Если изменилась уставка
            if(m_ustCur!=m_data.at(12)){
                if(flagUstSoftwared){//Если она изменилась из этой программы
                    if(loggingProperty) m_excelFile->writeToFile(ustSoftwared,m_data.at(12));
                    m_ustCur = m_data.at(12);
                    flagUstSoftwared=false;
                }
                else{//Если уставку поменяли из прибора
                    if(!m_timer->isActive())
                        m_timer->start();           //Запускаем таймер
                    m_ustCurTemp = m_data.at(12);   //Запоминаем кандидата на запись в лог
                }
            }
            //Изменилось число пробоев
            if(m_countBang!=m_data.at(16)){
                if(m_data.at(16) == 0)//Дефекты сбросили из прибора
                    m_excelFile->writeToFile(dropDeffectCountFromHardware,0);
                else                 //Обнаружили пробой
                    m_excelFile->writeToFile(countBangsChanged,m_data.at(16));
            }
        }
        else m_ustCur = m_data.at(12);
    }
    m_highCur = m_data.at(11);
    m_countBang = m_data.at(16);

    //Рисуем значения на экране
    ui->lcdNumber->display(m_ustCur/100.0);//Текущая уставка по напряжению
    ui->lcdNumber_2->display(m_voltageCur/100.0);//Текущее амплитудное напряжение
    ui->lcdNumber_3->display(m_countBang);//Количество пробоев

    //Данный регистр управляет большой красно-серой отображалкой
    if(m_highCur){
        ui->isHighLabel->setStyleSheet(red);
        ui->isHighLabel->setText("Высокое напряжение включено");  
    }
    else {
        ui->isHighLabel->setStyleSheet(gray);
        ui->isHighLabel->setText("Высокое напряжение отключено");
    }  
    //Данный регист управляет кнопкой
    if(m_isHigh){
        ui->onHighButton->setChecked(true);//Подтягиваем значение высокого на интерфейс
        ui->onHighButton->setText("Отключить");
    }
    else{
        ui->onHighButton->setChecked(false);
        ui->onHighButton->setText("Включить");
    }
    //Смысл в том, что если отключить защиту, то отображалка должна показать, что высокого нет.
    //Это записано в регистре 0x0B
    //Но кнопка при этом должна позволить выключить высокое, потому, что при подключении защиты
    //Высокое опять включится
}

void ZasiLooker::onConnect(bool state){
    if(state){
        m_excelFile = new ExcelWriter;
        m_excelFile->setFileName("log/ZASI log "+ QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".xlsx");
    }
    else{
        if(loggingProperty) m_excelFile->close();//Записываем финальное сообщение
        delete m_excelFile;  //Удаляем эксельку
        m_excelFile=nullptr; //Чистим указатель
        startSessionFlag = false;
    }
}

void ZasiLooker::on_setUstButton_clicked(){
    QVector<unsigned short> data;
    data.append(qRound(ui->setUstBox->value()*100));
    emit onSetReg(12, 1, data);
    flagUstSoftwared = true;
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
//Задержка по записи уставки
//Срабатывает, если за последнее время не меняли уставку.
void ZasiLooker::handlerTimer(){
     if(m_ustCurTemp != m_ustCur)//Если основную уставку еще не утверждали
       m_ustCur = m_ustCurTemp;     //Утверждаем и ждем следующего захода

     else{                      //Во второй раз, уставка уже утверждена, значит можно ее записывать
         m_excelFile->writeToFile(ustChanged,m_ustCur);
         m_timer->stop();
     }
}

void ZasiLooker::on_DropDefectCountButton_clicked(){
    QVector<unsigned short> data;
    data.append(0);
    emit onSetReg(16, 1, data);
    m_countBang=0;
    if(m_excelFile)
        m_excelFile->writeToFile(dropDeffectCount,0);
}
