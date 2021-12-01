#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow){
    ui->setupUi(this);

    HLayout = new QHBoxLayout();
    devicesLayout = new QHBoxLayout();
    VLayout = new QVBoxLayout();
    centralWidget()->setLayout(HLayout);

    m_connectionPanel = new ConnectionPanel(this);

    m_console = new Console(this);
    m_console->setMaximumHeight(300);
    ui->actionconsoleOn->setChecked(false);
    connect(m_connectionPanel,&ConnectionPanel::clearConsole,this,&MainWindow::clearConsole);

    //Создаем девайсы
    devices.append(new LDMDevice(this,m_connectionPanel->getIpAdd(0),m_connectionPanel->getPort(0),m_connectionPanel->getServer(0),ui->actiontcp_com));
    connectionStatuses.insert(m_connectionPanel->getServer(0),false);
    for (int i=1;i<m_counDevices;i++){
        if(ui->actiontcp_com)//tcp mode
            devices.append(new LDMDevice(this,m_connectionPanel->getIpAdd(i),m_connectionPanel->getPort(i),m_connectionPanel->getServer(i),ui->actiontcp_com));
        else
            devices.append(new LDMDevice(this,devices[0]->getModbusClient(),m_connectionPanel->getServer(i),ui->actiontcp_com));
        connectionStatuses.insert(m_connectionPanel->getServer(i),false);
    }


    m_statusBar = new StatusBar(ui->statusbar);

    HLayout->addLayout(VLayout);
    HLayout->addWidget(m_connectionPanel);
    VLayout->addLayout(devicesLayout);
    VLayout->addWidget(m_console);
    for(LDMDevice* i: devices){
        devicesLayout->addWidget(i);
    }


    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,this,&MainWindow::connectionPushed);//Когда нажата кнопка пробуем подл/откл
    connect(m_connectionPanel,&ConnectionPanel::serverChanged,[=](int numDev, int serverAdd){devices[numDev]->setServer(serverAdd);});//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::modelChanged,[=](int numDev, int model){devices[numDev]->setModel(model);});
    connect(m_connectionPanel,&ConnectionPanel::ipAdd_compChanged,[=](int numDev, const QString & ipAdd){devices[numDev]->setIpAdd_comp(ipAdd);});
    connect(m_connectionPanel,&ConnectionPanel::port_boudChanged,[=](int numDev, int port){devices[numDev]->setPort_boud(port);});

    for (LDMDevice* i:devices){
        connect(i,&LDMDevice::connectionStatus,this,&MainWindow::connectionChanged);                   //Состояние соединения изменилось
        connect(i,&LDMDevice::errorOccured, this,&MainWindow::connectionFailed);                       //Сообщение об ошибке
        connect(i,&LDMDevice::modbusRequestSent,this,&MainWindow::modbusReqPrint);                     //Печатаем запрос
        connect(i,&LDMDevice::modbusDataReceved,this,&MainWindow::modbusDataProcessing);               //Пришли данные по modbus
     }


    addToolBar(Qt::RightToolBarArea, ui->toolBar);//Перемещаем тулбарнаправо

}

MainWindow::~MainWindow()
{

    delete ui;
    for (LDMDevice *i:devices)
        delete i;
    delete m_statusBar;
    delete m_console;
}

void MainWindow::connectionChanged(int server, int status,const QString &host){
    QString str;
    //Проверяем, есть ли в мапе такой сервер и меняем ему статуст
    if(connectionStatuses.contains(server))
        connectionStatuses[server] = status;


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
    int statuses =0;
    for(QMap i:connectionStatuses){
         statuses+=i.value();
    }
    if(statuses == 0){//Все отключились
          //!!!&ConnectionPanel::connectionChanged//Когда подл/откл меняем кнопку
    }
    else if(statuses == connectionStatuses.size())){//Все подключились

    }


}

void MainWindow::connectionFailed( const QString &msg){
    QString str;
    m_statusBar->setMessageBar(msg);
    m_console->putData(str.toUtf8());
}

void MainWindow::connectionPushed(bool action){
//??? Здесь может быть ошибка. Для serial надо вызвать подключение только один раз
    if(action)  {
    for(LDMDevice* i:devices)
            i->onConnect();
    }
    else {
        for(LDMDevice* i:devices)
                i->onDisconnect();
    }
}

void MainWindow::modbusReqPrint(int server, const QByteArray &req){
    QString str = "REQ TO " +  QString::number(server) + " DEV: " + req;
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

