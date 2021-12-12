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
    m_connectionPanel->setPort(0, settings.value("port0").toInt());
    m_connectionPanel->setComport(settings.value("comport").toString());
    m_connectionPanel->setBaudrate(settings.value("baudrate").toInt());
    //m_connectionPanel->setModel(0,settings.value("model0").toInt());
    m_connectionPanel->setServer(0,settings.value("server0").toInt());

    //Создаем девайсы
    devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(0),ConnectionType::Serial,m_connectionPanel->getModel(0),DeviceType::ZASI));
    devicesLayout->addWidget(devices.at(0));

    if(settings.value("interfaceMode").toBool() == 0)//Проверяем тип девайсов
        connectNewDevice(devices.at(0));            //Если Serial, просто вызываем коннекты, т.к. он уже создан
    else
        ui->actiontcp_com->setChecked(true);        //Иначе нажимаем кнопку смены интерфейса
    settings.endGroup();

    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,this,&MainWindow::connectionPushed);//Когда нажата кнопка пробуем подл/откл
    connect(m_connectionPanel,&ConnectionPanel::serverChanged,[=](int numDev, int serverAdd){ devices[numDev]->setServer(serverAdd);});//Изменился сервер
    connect(m_connectionPanel,&ConnectionPanel::modelChanged,[=](int numDev, int model){ devices[numDev]->setModel(model);});
    connect(m_connectionPanel,&ConnectionPanel::ipAdd_compChanged,[=](int numDev, const QString & ipAdd){ devices[numDev]->setIpAdd_comp(ipAdd);});
    connect(m_connectionPanel,&ConnectionPanel::port_boudChanged,[=](int numDev, int port){ devices[numDev]->setPort_boud(port);});
    connect(m_connectionPanel,&ConnectionPanel::logChekingChanged,[=](bool state){ for(auto i:devices) ;});

    m_statusBar = new StatusBar(ui->statusbar);

    addToolBar(Qt::RightToolBarArea, ui->toolBar);//Перемещаем тулбарнаправо

    ui->actiondoubleMode->setVisible(false);
}

MainWindow::~MainWindow()
{
    saveSettings();
    for (LDMDevice *i:devices)
        delete i;
    delete m_statusBar;
    delete m_console;
    delete m_connectionPanel;
    delete ui;
}
//Прописывае коннекты для нового девайса
void MainWindow::connectNewDevice(LDMDevice *dev){
    connect(dev,&LDMDevice::connectionStatus,this,&MainWindow::connectionChanged);                   //Состояние соединения изменилось
    connect(dev,&LDMDevice::errorOccured, this,&MainWindow::connectionFailed);                       //Сообщение об ошибке
    connect(dev,&LDMDevice::modbusRequestSent,this,&MainWindow::modbusPacketPrint);                //Печатаем запрос
    connect(dev,&LDMDevice::modbusDataReceved,this,&MainWindow::modbusPacketPrint);               //Пришли данные по modbus
    connect(dev,&LDMDevice::modelChanged,m_connectionPanel,[=](int model){m_connectionPanel->setModel(0,model);});
}

void MainWindow::saveSettings(){
    //Запись в файл настроек
      QSettings settings("settings.ini",QSettings::IniFormat);
      settings.beginGroup("Settings");
      settings.setValue( "ip0", m_connectionPanel->getIpAdd(0));
      settings.setValue( "port0", m_connectionPanel->getPort(0));
      settings.setValue( "comport", m_connectionPanel->getComport());
      settings.setValue( "baudrate", m_connectionPanel->getBaud());
      settings.setValue( "model0", m_connectionPanel->getModel(0));
      settings.setValue( "server0", m_connectionPanel->getServer(0));
      settings.setValue( "interfaceMode", ui->actiontcp_com->isChecked());

      settings.endGroup();
}
//Изменен статус подключения. Сигнал из lmdDevice
void MainWindow::connectionChanged(int server, int status,const QString &host){
    QString str;

    switch(status){
        case 0:   //Отключено
            str = "Отключено от " + host + "\n";
            m_statusBar->setMessageBar(str);            
            m_connectionPanel->setStatusLabel(server,false);
            m_console->putData(str.toUtf8());
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
            break;
        case 3:       //Отключение
            str = "Отключение от " + host + "\n";
            m_statusBar->setMessageBar(str);
            m_console->putData(str.toUtf8());
            ui->actiondoubleMode->setEnabled(false);
            break;
    }

    //Если в мапе нет сервера, создаем его и записываем его состояние
    //Если в мапе есть этот сервер, то изменяем его состояние
    if(!connectionState.contains(server))
        connectionState.insert(server,status);
    else
        connectionState[server] = status;

    connectedDevices = 0;
    connectingDevice=0;
    //Считаем сколько и каких у нас устройств
    for(auto e : connectionState.keys()){
      if(connectionState.value(e) == 2)
          connectedDevices++;
      else if(connectionState.value(e) == 1 || connectionState.value(e) == 3)
          connectingDevice++;
    }

    if(connectedDevices==0){//Нет подключеных устройств
        if(connectingDevice==0){//Нет подключающихся устройств
            connectionState.clear(); //Когда все устройста отключены, чистим мап, потому что сервера могут поменяться
            ui->actiondoubleMode->setEnabled(true);
            ui->actiontcp_com->setEnabled(true);
            m_connectionPanel->connectionButtonChanged(true,0);//ПОДКЛЮЧИТЬСЯ вкл
            m_connectionPanel->enablePanel(true);              //панель вкл

        }
        else{//Процесс  отключения/подключения
            ui->actiondoubleMode->setEnabled(false);
            ui->actiontcp_com->setEnabled(false);
            m_connectionPanel->connectionButtonChanged(false,2);//Текст не менять выкл
            m_connectionPanel->enablePanel(false);              //панель выкл
        }
    }
    else{
        ui->actiondoubleMode->setEnabled(false);
        ui->actiontcp_com->setEnabled(false);
        m_connectionPanel->enablePanel(false);                  //панель выкл
        if(connectedDevices == currentCountDev){                //Все подключились
            m_connectionPanel->connectionButtonChanged(true,1);     //ОТКЛЮЧИТЬСЯ вкл
        }
        else{                                                   //Не все существующие устройства подключены
            if(connectingDevice==0)                                 //Нет подключающихся устройств
                m_connectionPanel->connectionButtonChanged(true,1);     //ОТКЛЮЧИТЬСЯ вкл
            else                                                    //Процесс подключения отключения отключения/подключения
                m_connectionPanel->connectionButtonChanged(false,2);    //Текст не менять выкл
        }
    }
}
//Печать сообщения об ошибке в строку статуса и консоль
//Сигнал из lmd Device
void MainWindow::connectionFailed( const QString &msg){
    m_statusBar->setMessageBar(msg);
    m_console->putData(msg.toUtf8());
}

//Нажата кнопка подключиться/отключиться
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
//Печатаем пакет модбас
void MainWindow::modbusPacketPrint(int , const QString &str){
    m_console->putData(str.toUtf8());
}
//Очистить консоль по нажатию кнопки
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
            devices.append(new LDMDevice(this,m_connectionPanel->getIpAdd(i),m_connectionPanel->getPort(i),m_connectionPanel->getServer(i),ConnectionType::Tcp,m_connectionPanel->getModel(i),DeviceType::ZASI));
            devicesLayout->addWidget(devices.at(i));
            connectNewDevice(devices.at(i));
        }
    }
    else{
        devices.append(new LDMDevice(this,m_connectionPanel->getComport(),m_connectionPanel->getBaud(),m_connectionPanel->getServer(0),ConnectionType::Serial,m_connectionPanel->getModel(0),DeviceType::ZASI));
        devicesLayout->addWidget(devices.at(0));
        connectNewDevice(devices.at(0));
        for( int i = 1; i<currentCountDev;i++){
            devices.append(new LDMDevice(this,devices[0]->getModbusClient(),m_connectionPanel->getServer(i),ConnectionType::Serial,m_connectionPanel->getModel(i),DeviceType::ZASI));
            devicesLayout->addWidget(devices.at(i));
            connectNewDevice(devices.at(i));
        }
    }
}
//Показать окно помощи
void MainWindow::on_actionHelp_triggered(){
    QMessageBox::information(this,"Помощь!","Приветствуем Вас в программе <b>ZASIViewer!</b> <br>"
                                            "Данное приложение создано для контроля и управления приборами серии ЗАСИ.<br>"
                                            "<br>"
                                            "<b>Для работы с программой необходимо:</b><br>"
                                            "1. Подключить прибор к компьютеру через преобразователь RS-485|USB или Ethernet-порт.<br>"
                                            "2. Выбрать интерфейс подключения (Serial-485 / Ethernet) с помощью кнопки на боковой панели инструментов справа.<br>"
                                            "3. Настроить интерфейс подключения. Для Serial-485 выбрать COM-порт и скорость обмена (по умолчанию 9600). "
                                            "Для Ethernet установить ip-адрес и порт (указаны в инструкции по эксплуатации).<br>"
                                            "4. Настроить адрес устройства.<br>"
                                            "5. Для записи лога работы установите отметку в поле \"Логировать в Excel\". Лог будет сохранен в папке \"log\", находящейся в папке с программой.<br>"
                                            "6. Нажать кнопку \"Подключиться\".<br>"
                                            "<br>"
                                            "<b>Функции управления прибором:</b><br>"
                                            "1. Кнопка \"Установить\" задает введенную уставку в прибор.<br>"
                                            "2. Кнопка \"Включить/Выключить\" включает/выключает высокое напряжение в приборе.<br>"
                                            "3. Кнопка \"Сбросить\" устанавливает количество пробоев равным нулю.<br>"
                                            "<br>"
                                            "<b>Дополнительно:</b><br>"
                                            "- Справа от адреса устройства отображается текущий статус интерфеса подключения.<br>"
                                            "- Для изменения интерфейса подключения необходимо предварительно отключиться.<br>"
                                            "- Кнопка \"Отображать консоль\" на панели управления справа  скрывает/отоборажает отображение сервисной информации.<br>"
                                            "- Следующая за ней кнопка \"Отображать панель настроек\" скрывает/отоборажает настройки подключения.<br>");
}

