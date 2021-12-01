#include "diameterlooker.h"
#include "ui_diameterlooker.h"

diameterLooker::diameterLooker(QWidget *parent, int num, int diam) :  QGroupBox(parent), ui(new Ui::diameterLooker)
{
    ui->setupUi(this);
    setTitle("Прибор №" + QString::number(num));
    m_diam = diam;
    m_centerViewer = new centerViewer(this,m_diam);
    ui->HLayout->addWidget(m_centerViewer);
    QPalette palette;
    palette.setColor(QPalette::WindowText,Qt::red);
    ui->lcdNumber->setPalette(palette);
}

diameterLooker::~diameterLooker()
{
    delete ui;
    delete m_centerViewer;
}

void diameterLooker::setData(const QVector<double> &data)
{
    m_data = data;
    ui->lcdNumber->display(m_data.at(0));//Основной диаметр
    ui->lcdNumber_2->display(m_data.at(1));//х Диаметр
    ui->lcdNumber_3->display(m_data.at(2));//y Диаметр
    m_centerViewer->setCoord(m_data.at(3),m_data.at(4));   //Размер и положение
    m_centerViewer->setRad(m_data.at(1),m_data.at(2));
    setError(m_data.at(5));
}

void diameterLooker::setDiam(int diam)
{
    m_diam = diam;
    m_centerViewer->setScale(m_diam);
}

void diameterLooker::setName(int server){
    setTitle("Прибор №" + QString::number(server));
}

void diameterLooker::setError(int error){
     QString error_message;
     switch (error) {
               case 0: error_message = tr("-");break;
               case 1: error_message = tr("Перекрытие рабочей зоны по двум каналам."); break;
               case 2: error_message = tr("Перекрытие верхней границы рабочей зоны по двум каналам"); break;
               case 3:error_message = tr("Перекрытие нижней границы рабочей зоны по двум каналам");break;
               case 4: error_message = tr("Перекрытие нижней границы рабочей зоны по первому каналу");break;
               case 5: error_message = tr("Перекрытие верхней границы рабочей зоны по первому каналу");break;
               case 6: error_message = tr("Перекрытие рабочей зоны по первому каналу.");break;
               case 7: error_message = tr("Перекрытие верхней границы рабочей зоны по второму каналу");break;
               case 8:error_message = tr("Перекрытие нижней границы рабочей зоны по второму каналу");break;
               case 9: error_message = tr("Перекрытие рабочей зоны по второму каналу.");break;
               case 10: error_message = tr("Перекрытие рабочей зоны по первому каналу и перекрытие верхней границы рабочей зоны по второму каналу"); break;
               case 11: error_message = tr("Перекрытие рабочей зоны по первому каналу перекрытие нижней границы рабочей зоны по второму каналу.");break;
               case 12: error_message = tr("Перекрытие рабочей зоны по второму каналу и перекрытие верхней границы рабочей зоны по первому каналу."); break;
               case 13: error_message = tr("Перекрытие рабочей зоны по второму каналу и перекрытие нижней границы рабочей зоны по первому каналу."); break;
               case 14: error_message = tr("Перекрытие верхней границы рабочей зоны по первому каналу и перекрытие нижней границы рабочей зоны по второму каналу."); break;
               case 15:error_message = tr("Перекрытие нижней границы рабочей зоны по первому каналу и перекрытие верхней границы рабочей зоны по второму каналу.");break;
               case 16: error_message = tr("Наличие двух и более объектов в зоне измерения.");break;
               default: error_message = tr("Неизвестная ошибка"); break;
      }
     ui->label->setText(error_message);
     if(error!=0){
         ui->lcdNumber->setPalette(Qt::red);
         ui->lcdNumber_2->setPalette(Qt::red);
         ui->lcdNumber_3->setPalette(Qt::red);

     }
     else{
         ui->lcdNumber->setPalette(Qt::black);
         ui->lcdNumber_2->setPalette(Qt::black);
         ui->lcdNumber_3->setPalette(Qt::black);
     }
}
