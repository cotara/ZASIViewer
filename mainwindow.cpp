#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow){
    ui->setupUi(this);

    HLayout = new QHBoxLayout();
    VLayout = new QVBoxLayout();
    centralWidget()->setLayout(HLayout);

    m_connectionPanel = new ConnectionPanel(this);
    m_console = new Console(this);
    m_console->setMaximumHeight(300);
    //m_console->setMinimumHeight(200);
    connect(m_connectionPanel,&ConnectionPanel::clearConsole,this,&MainWindow::clearConsole);

    m_looker = new Looker(this);
    m_looker->setMinimumHeight(500);
    m_statusBar = new StatusBar(ui->statusbar);

    HLayout->addLayout(VLayout);
    HLayout->addWidget(m_connectionPanel);

    VLayout->addWidget(m_looker);
    VLayout->addWidget(m_console);


    m_connectionPanel->setMaximumWidth(250);

    m_modbusClient = new ModBusClient(this);

    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,m_modbusClient,&ModBusClient::onConnect);//Когда нажата кнопка пробуем подл/откл
    connect(m_modbusClient,&ModBusClient::connectionStatus, m_connectionPanel,&ConnectionPanel::connectionChanged);//Когда подл/откл меняем кнопку
    connect(m_modbusClient,&ModBusClient::connectionStatus, this,&MainWindow::connectionChanged);               //Когда подл/откл выводим в бар
    connect(m_modbusClient,&ModBusClient::errorOccured, this,&MainWindow::connectionFailed);               //Сообщение об ошибке
    connect(m_modbusClient,&ModBusClient::modbusRequestSent,this,&MainWindow::modbusReqPrint);              //Печатаем запрос
    connect(m_modbusClient,&ModBusClient::modbusDataReceved,this,&MainWindow::modbusDataProcessing);                   //Пришли данные по modbus
    connect(m_connectionPanel,&ConnectionPanel::connectionTypeChanged,m_modbusClient,&ModBusClient::onConnectTypeChanged);//Изменился тип подключения

    connect(m_connectionPanel,&ConnectionPanel::doubleModeChanged,m_modbusClient,&ModBusClient::setDoubleMode);
    connect(m_connectionPanel,&ConnectionPanel::server1changed,m_modbusClient,&ModBusClient::setServer1);
    connect(m_connectionPanel,&ConnectionPanel::server2changed,m_modbusClient,&ModBusClient::setServer2);
    connect(m_connectionPanel,&ConnectionPanel::server1changed,m_looker,&Looker::setNumDev1);
    connect(m_connectionPanel,&ConnectionPanel::server2changed,m_looker,&Looker::setNumDev2);
    connect(m_connectionPanel,&ConnectionPanel::diameter1changed,m_looker,&Looker::setDiam1);
    connect(m_connectionPanel,&ConnectionPanel::diameter2changed,m_looker,&Looker::setDiam2);

    m_modbusClient->setServer1(m_connectionPanel->getDevNum(1));//Инициализация лукера и момбаса
    m_modbusClient->setServer2(m_connectionPanel->getDevNum(2));
    m_looker->setNumDev1(m_connectionPanel->getDevNum(1));
    m_looker->setNumDev2(m_connectionPanel->getDevNum(2));
    m_looker->setDiam1(m_connectionPanel->getDiam(1));
    m_looker->setDiam2(m_connectionPanel->getDiam(2));
}

MainWindow::~MainWindow()
{

    delete ui;
    delete m_modbusClient;

    delete m_looker;
    delete m_statusBar;
    delete m_console;
}

void MainWindow::connectionChanged(int status,const QString &host){
    QString str;
    switch(status){
        case 0:   //Отключено
            str = "Отключено от " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            break;
        case 1:    //Подключение
           str = "Подключение к " + host + "\n";
           m_statusBar->setMessageBar(str);
           m_console->putData(str.toUtf8());
        break;
        case 2:     //Подключено
            str = "Подключено к " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            break;
        case 3:       //Отключение
            str = "Отключение от " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            break;
    }
}

void MainWindow::connectionFailed(int numDev,const QString &msg){
    QString str;
    str = "Устройство c адресом " + QString::number(numDev) + " отключено. " + msg + "\n";
    m_statusBar->setMessageBar(str);
    m_console->putData(str.toUtf8());
}

void MainWindow::modbusReqPrint(int numDev, const QByteArray &req){
    QString str = "REQ TO " +  QString::number(numDev) + " DEV: " + req;
    m_console->putData(str.toUtf8());
}

//Обработчки данных, пришедших от устройства
void MainWindow::modbusDataProcessing(int numDev, const QVector<unsigned short>& data){
    QString str = "REPLY FROM " +  QString::number(numDev) + " DEV: ";

    for(short i:data)
        str+=QString::number(i) + " ";

    str+="\n";
    m_console->putData(str.toUtf8());

    QVector<double> doubleData, tempData;

    if(data.size()!=0){
        doubleData.append(static_cast<double>(data.at(1)+65536*data.at(0))/1000.0);
        doubleData.append(static_cast<double>(data.at(3)+65536*data.at(2))/1000.0);
        doubleData.append(static_cast<double>(data.at(5)+65536*data.at(4))/1000.0);
        doubleData.append(static_cast<double>(60000-data.at(9)-65536*data.at(8))/1000.0);
        doubleData.append(static_cast<double>(60000-data.at(11)-65536*data.at(10))/1000.0);

        m_looker->setData(doubleData,numDev);
    }
}

void MainWindow::clearConsole(){
    m_console->clear();
}

