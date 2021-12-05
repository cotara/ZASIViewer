#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QPropertyAnimation>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow){
    ui->setupUi(this);

    HLayout = new QHBoxLayout();
    devicesLayout = new QHBoxLayout();
    VLayout = new QVBoxLayout();
    centralWidget()->setLayout(HLayout);
    //Панель подключения
    m_connectionPanel = new ConnectionPanel(this);
    m_connectionPanel->setMaximumWidth(300);
    //Консоль
    m_console = new Console(this);
    m_console->setMaximumHeight(250);
    ui->actionconsoleOn->setChecked(false);
    connect(m_connectionPanel,&ConnectionPanel::clearConsole,this,&MainWindow::clearConsole);
    //Добавление виджетов
    HLayout->addLayout(VLayout);
    HLayout->addWidget(m_connectionPanel);
    VLayout->addLayout(devicesLayout);
    VLayout->addWidget(m_console);

        //Чтение настроек

    QSettings settings ("settings.ini",QSettings::IniFormat);
    settings.beginGroup("Settings");

    //Считывание настроек необходимо делать до коннектов, чтобы задать только значения в панели управления
    m_connectionPanel->setIP(0,settings.value("ip0").toString());
    m_connectionPanel->setIP(1,settings.value("ip1").toString());
    m_connectionPanel->setPort(0, settings.value("port0").toInt());
    m_connectionPanel->setPort(1, settings.value("port1").toInt());
    m_connectionPanel->setComport(settings.value("comport").toString());
    m_connectionPanel->setBaudrate(settings.value("baudrate").toInt());
    m_connectionPanel->setModel(0,settings.value("model0").toInt());
    m_connectionPanel->setModel(1,settings.value("model1").toInt());
    m_connectionPanel->setServer(0,settings.value("server0").toInt());
    m_connectionPanel->setServer(1,settings.value("server1").toInt());

        //Создаем девайсы
    devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(0),ConnectionType::Serial,m_connectionPanel->getModel(0),DeviceType::LDM));
    devicesLayout->addWidget(devices.at(0));
    m_statusBar = new StatusBar(ui->statusbar);

    //Проверка был ли включен двойной режим необходмо делать после создания первого девайса, чтобы можно было добавить второй.
    //on_actiondoubleMode_toggled();
    ui->actiondoubleMode->setChecked(settings.value("doubleMode").toBool());//Проверяем сколько девайсов было в настройках
    ui->actiontcp_com->setChecked(settings.value("interfaceMode").toBool());//Проверяем тип девайсов
    settings.endGroup();

    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,this,&MainWindow::connectionPushed);//Когда нажата кнопка пробуем подл/откл
    connect(m_connectionPanel,&ConnectionPanel::serverChanged,[=](int numDev, int serverAdd){ devices[numDev]->setServer(serverAdd);});//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::modelChanged,[=](int numDev, int model){ devices[numDev]->setModel(model);});
    connect(m_connectionPanel,&ConnectionPanel::ipAdd_compChanged,[=](int numDev, const QString & ipAdd){ devices[numDev]->setIpAdd_comp(ipAdd);});
    connect(m_connectionPanel,&ConnectionPanel::port_boudChanged,[=](int numDev, int port){ devices[numDev]->setPort_boud(port);});

    connectNewDevice(devices[0]);

    addToolBar(Qt::RightToolBarArea, ui->toolBar);//Перемещаем тулбарнаправо



}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
    for (LDMDevice *i:devices)
        delete i;
    delete m_statusBar;
    delete m_console;
    delete m_connectionPanel;
}

void MainWindow::connectNewDevice(LDMDevice *dev){
    connect(dev,&LDMDevice::connectionStatus,this,&MainWindow::connectionChanged);                   //Состояние соединения изменилось
    connect(dev,&LDMDevice::errorOccured, this,&MainWindow::connectionFailed);                       //Сообщение об ошибке
    connect(dev,&LDMDevice::modbusRequestSent,this,&MainWindow::modbusPacketPrint);                //Печатаем запрос
    connect(dev,&LDMDevice::modbusDataReceved,this,&MainWindow::modbusPacketPrint);               //Пришли данные по modbus
}

void MainWindow::saveSettings(){
    //Запись в файл настроек
      QSettings settings("settings.ini",QSettings::IniFormat);
      settings.beginGroup("Settings");
      settings.setValue( "ip0", m_connectionPanel->getIpAdd(0));
      settings.setValue( "ip1", m_connectionPanel->getIpAdd(1));
      settings.setValue( "port0", m_connectionPanel->getPort(0));
      settings.setValue( "port1", m_connectionPanel->getPort(1));
      settings.setValue( "comport", m_connectionPanel->getComport());
      settings.setValue( "baudrate", m_connectionPanel->getBaud());
      settings.setValue( "model0", m_connectionPanel->getModel(0));
      settings.setValue( "model1", m_connectionPanel->getModel(1));
      settings.setValue( "server0", m_connectionPanel->getServer(0));
      settings.setValue( "server1", m_connectionPanel->getServer(1));
      settings.setValue( "doubleMode", ui->actiondoubleMode->isChecked());
      settings.setValue( "interfaceMode", ui->actiontcp_com->isChecked());

      settings.endGroup();

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
           connectedDevices++;
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
            connectedDevices--;
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
    m_connectionPanel->enableClearConsoleButton(arg1);
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
            devices.append(new LDMDevice(this,devices[0]->getModbusClient(),m_connectionPanel->getServer(i),ConnectionType::Serial,m_connectionPanel->getModel(i),DeviceType::LDM));
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
        if(devices.size()>1){//Удалять можно только, если 2 и более устройств
            devicesLayout->removeWidget(devices.at(1));
            currentCountDev--;
            delete devices.at(1);
            devices.remove(1);
        }
    }
}


