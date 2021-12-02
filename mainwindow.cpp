#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow){
    ui->setupUi(this);

    HLayout = new QHBoxLayout();
    devicesLayout = new QHBoxLayout();
    VLayout = new QVBoxLayout();
    centralWidget()->setLayout(HLayout);
    //Панель подключения
    m_connectionPanel = new ConnectionPanel(this);
    //Консоль
    m_console = new Console(this);
    m_console->setMaximumHeight(300);
    ui->actionconsoleOn->setChecked(false);
    connect(m_connectionPanel,&ConnectionPanel::clearConsole,this,&MainWindow::clearConsole);
    //Добавление виджетов
    HLayout->addLayout(VLayout);
    HLayout->addWidget(m_connectionPanel);
    VLayout->addLayout(devicesLayout);
    VLayout->addWidget(m_console);

        //Создаем девайсы
    devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(0),ConnectionType::Serial,m_connectionPanel->getModel(0),DeviceType::LDM));
    devicesLayout->addWidget(devices.at(0));
    m_statusBar = new StatusBar(ui->statusbar);

    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,this,&MainWindow::connectionPushed);//Когда нажата кнопка пробуем подл/откл
    connect(m_connectionPanel,&ConnectionPanel::serverChanged,[=](int numDev, int serverAdd){devices[numDev]->setServer(serverAdd);});//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::modelChanged,[=](int numDev, int model){devices[numDev]->setModel(model);});
    connect(m_connectionPanel,&ConnectionPanel::ipAdd_compChanged,[=](int numDev, const QString & ipAdd){devices[numDev]->setIpAdd_comp(ipAdd);});
    connect(m_connectionPanel,&ConnectionPanel::port_boudChanged,[=](int numDev, int port){devices[numDev]->setPort_boud(port);});

    connectNewDevice(devices[0]);

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

void MainWindow::connectNewDevice(LDMDevice *dev){
    connect(dev,&LDMDevice::connectionStatus,this,&MainWindow::connectionChanged);                   //Состояние соединения изменилось
    connect(dev,&LDMDevice::errorOccured, this,&MainWindow::connectionFailed);                       //Сообщение об ошибке
    connect(dev,&LDMDevice::modbusRequestSent,this,&MainWindow::modbusPacketPrint);                //Печатаем запрос
    connect(dev,&LDMDevice::modbusDataReceved,this,&MainWindow::modbusPacketPrint);               //Пришли данные по modbus
}

void MainWindow::connectionChanged(int server, int status,const QString &host){
    QString str;

    switch(status){
        case 0:   //Отключено
            str = "Отключено от " + host + "\n";
            m_statusBar->setMessageBar(str);            
            m_connectionPanel->setStatusLabel(server,false);
            m_console->putData(str.toUtf8());
            connectedDevices--;
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
            m_connectionPanel->setStatusLabel(server,true);
            m_console->putData(str.toUtf8());
            ui->actiondoubleMode->setEnabled(false);
            connectedDevices++;
            break;
        case 3:       //Отключение
            str = "Отключение от " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            ui->actiondoubleMode->setEnabled(false);
            break;
    }

    if(connectedDevices==0){//Все устройства отключились
        ui->actiondoubleMode->setEnabled(true);
        m_connectionPanel->connectionChanged(0);
    }
    else if(connectedDevices == currentCountDev){//Все подключились
        ui->actiondoubleMode->setEnabled(false);
        m_connectionPanel->connectionChanged(1);
    }
    else{//Происходит процесс подключения
        ui->actiondoubleMode->setEnabled(false);
        m_connectionPanel->connectionChanged(2);
    }
}

void MainWindow::connectionFailed( const QString &msg){
    QString str;
    m_statusBar->setMessageBar(msg);
    m_console->putData(str.toUtf8());
}

void MainWindow::connectionPushed(bool action){
    if(action)  {
        for(LDMDevice* i:devices)
                i->onConnect();
    }
    else {
        for(LDMDevice* i:devices)
                i->onDisconnect();
    }
}

void MainWindow::modbusPacketPrint(int , const QString &str){
    m_console->putData(str.toUtf8());
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
    //При смене типа надо заново создавать все устройства, поскольку в конструкторе передаются все нужные параметры (адреса, скорости и прочее

    for(LDMDevice * i: devices){
        devicesLayout->removeWidget(i);
        delete i;
    }

    devices.clear();
    if(arg1 == Tcp){
        for(int i =0;i<currentCountDev;i++){
            devices.append(new LDMDevice(this,m_connectionPanel->getIpAdd(i),m_connectionPanel->getPort(i),m_connectionPanel->getServer(i),ConnectionType::Tcp,m_connectionPanel->getModel(i),DeviceType::LDM));
            devicesLayout->addWidget(devices.at(i));
            connectNewDevice(devices.at(i));
        }
    }
    else{
        devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(0),ConnectionType::Serial,m_connectionPanel->getModel(0),DeviceType::LDM));
        devicesLayout->addWidget(devices.at(0));
        connectNewDevice(0);
        for( int i = 1; i<currentCountDev;i++){
            devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(i),ConnectionType::Serial,m_connectionPanel->getModel(i),DeviceType::LDM));
            devicesLayout->addWidget(devices.at(i));
            connectNewDevice(devices.at(i));
        }
    }
}

//Переключение 1-2 устройства
void MainWindow::on_actiondoubleMode_toggled(bool arg1){
    m_connectionPanel->setDoubleMode(arg1);

    if(arg1){//Если надо добавить второго
        if(ui->actiontcp_com->isChecked())//tcp mode
            devices.append(new LDMDevice(this,m_connectionPanel->getIpAdd(1),m_connectionPanel->getPort(1),m_connectionPanel->getServer(1),ConnectionType::Tcp,m_connectionPanel->getModel(1),DeviceType::LDM));
        else
            devices.append(new LDMDevice(this,devices[0]->getModbusClient(),m_connectionPanel->getServer(1),ConnectionType::Serial,m_connectionPanel->getModel(1),DeviceType::LDM));

        connectNewDevice(devices[1]);
        devicesLayout->addWidget(devices.at(1));
        currentCountDev++;
    }
    else{
        devicesLayout->removeWidget(devices.at(1));
        currentCountDev--;
        delete devices.at(1);
        devices.remove(1);
    }
}


