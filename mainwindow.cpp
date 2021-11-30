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
    ui->actionconsoleOn->setChecked(false);
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
    connect(m_connectionPanel,&ConnectionPanel::connectionTypeChanged,m_modbusClient,&ModBusClient::onConnectTypeChanged);//Изменился тип подключения
    connect(m_connectionPanel,&ConnectionPanel::server1changed,m_modbusClient,&ModBusClient::setServer1);//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::server2changed,m_modbusClient,&ModBusClient::setServer2);
    connect(m_connectionPanel,&ConnectionPanel::server1changed,m_looker,&Looker::setNumDev1);//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::server2changed,m_looker,&Looker::setNumDev2);
    connect(m_connectionPanel,&ConnectionPanel::diameter1changed,m_looker,&Looker::setDiam1);//Изменился диаметр (модель
    connect(m_connectionPanel,&ConnectionPanel::diameter2changed,m_looker,&Looker::setDiam2);

    connect(m_modbusClient,&ModBusClient::connectionStatus, m_connectionPanel,&ConnectionPanel::connectionChanged);//Когда подл/откл меняем кнопку
    connect(m_modbusClient,&ModBusClient::connectionStatus, this,&MainWindow::connectionChanged);                  //Когда подл/откл выводим в бар
    connect(m_modbusClient,&ModBusClient::errorOccured, this,&MainWindow::connectionFailed);                       //Сообщение об ошибке
    connect(m_modbusClient,&ModBusClient::modbusRequestSent,this,&MainWindow::modbusReqPrint);                     //Печатаем запрос
    connect(m_modbusClient,&ModBusClient::modbusDataReceved,this,&MainWindow::modbusDataProcessing);               //Пришли данные по modbus

    m_modbusClient->setServer1(m_connectionPanel->getServer(1));//Инициализация лукера и момбаса
    m_modbusClient->setServer2(m_connectionPanel->getServer(2));
    m_looker->setNumDev1(m_connectionPanel->getServer(1));
    m_looker->setNumDev2(m_connectionPanel->getServer(2));
    m_looker->setDiam1(m_connectionPanel->getDiam(1));
    m_looker->setDiam2(m_connectionPanel->getDiam(2));

    addToolBar(Qt::RightToolBarArea, ui->toolBar);//Перемещаем тулбарнаправо

}

MainWindow::~MainWindow()
{

    delete ui;
    delete m_modbusClient;

    delete m_looker;
    delete m_statusBar;
    delete m_console;
}

void MainWindow::connectionChanged(int server, int status,const QString &host){
    QString str;
    switch(status){
        case 0:   //Отключено
            str = "Отключено от " + host + "\n";
            m_statusBar->setMessageBar(str);            
            m_statusBar->setStatus(false);
            m_console->putData(str.toUtf8());
            m_looker->setEnabled(server,false);
            ui->actiondoubleMode->setEnabled(true);
            break;
        case 1:    //Подключение
           str = "Подключение к " + host + "\n";
           m_statusBar->setMessageBar(str);
           m_console->putData(str.toUtf8());
           ui->actiondoubleMode->setEnabled(false);
        break;
        case 2:     //Подключено
            str = "Подключено к " + host + "\n";
            m_statusBar->setMessageBar(str);            
            m_statusBar->setStatus(true);
            m_console->putData(str.toUtf8());
            ui->actiondoubleMode->setEnabled(false);
            break;
        case 3:       //Отключение
            str = "Отключение от " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            ui->actiondoubleMode->setEnabled(false);
            break;
    }
}

void MainWindow::connectionFailed(const QString &msg){
    QString str;
    m_statusBar->setMessageBar(msg);
    m_console->putData(str.toUtf8());
    m_looker->setEnabled(numDev,false);
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
        m_looker->setEnabled(numDev,true);
        doubleData.append((data.at(1)+65536*data.at(0))/1000.0);
        doubleData.append((data.at(3)+65536*data.at(2))/1000.0);
        doubleData.append((data.at(5)+65536*data.at(4))/1000.0);
        doubleData.append((60000-data.at(9)-65536*data.at(8))/1000.0);
        doubleData.append((60000-data.at(11)-65536*data.at(10))/1000.0);
        doubleData.append((data.at(13)+65536*data.at(12)));
        m_looker->setData(doubleData,numDev);
    }
    else{
        m_looker->setEnabled(numDev,false);
    }
}

void MainWindow::clearConsole(){
    m_console->clear();
}


//Показать консоль
void MainWindow::on_actionconsoleOn_toggled(bool arg1){
    m_console->setVisible(arg1);
}

//Показать настройки
void MainWindow::on_actionsettingsOn_toggled(bool arg1){
    m_connectionPanel->setVisible(arg1);
}

//Переключение tcp|COM
void MainWindow::on_actiontcp_com_toggled(bool arg1){
    m_connectionPanel->setInterface(arg1);
}

//Переключение 1-2 устройства
void MainWindow::on_actiondoubleMode_toggled(bool arg1){
    m_connectionPanel->setDoubleMode(arg1);
    m_modbusClient->setDoubleMode(arg1);
    m_looker->enableSecondLooker(arg1);

}

