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

    //Консоль
    m_console = new Console(this);
    m_console->setMaximumHeight(300);
    on_actionconsoleOn_toggled(false);

    //Статус бар
    m_statusBar = new StatusBar(ui->statusbar);
    m_statusBar->visibleResent(true);
    m_statusBar->visibleStatus(true);
    m_statusBar->visibleBar(true);
    m_statusBar->setDownloadBarFormat("%v Гц");
    m_statusBar->setDownloadBarRange(30);
    m_statusBar->visibleTime(true);
    m_statusBar->visibleAnimation(true);

    //Добавление виджетов
    HLayout->addLayout(VLayout);
    VLayout->addLayout(devicesLayout);
    VLayout->addWidget(m_console);

    //Настройки
    m_serialSetting = new SerialSettings(this);
    m_conSettings = m_serialSetting->getSettingsList().at(0);   //Настройки, прочитанные из файла
    connect(m_serialSetting, &SerialSettings::conSettingsChanged,this, &MainWindow::newSettingsOk);
    m_serialSetting->setType("ZASI");
    m_serialSetting->setEnabledType(false);
    m_serialSetting->setSearchVisible(false);
    //Создаем девайс
    createRedwillDevice(m_conSettings.interf);

    fillDevInfo();

    //Создаем лукер
    m_looker = new ZasiLooker(this,m_conSettings.host, m_devInfo.model,m_devInfo.server);
    connect(m_looker, &ZasiLooker::setParams, this,[=](const QVector<unsigned short> & par, int startAdd, int count){
        add6ToQueue(m_looker->server(),par, startAdd,count);
    });
    devicesLayout->addWidget(m_looker);

    //Таймер отправки
    m_timerSend = new QTimer(this);
    m_timerSend->setInterval(m_period);
    connect(m_timerSend,&QTimer::timeout,this, &MainWindow::sendTimeout);

    //Таймер подсчета частоты
    m_freqCalcTimer = new QTimer(this);
    connect(m_freqCalcTimer,&QTimer::timeout,this, &MainWindow::freqCalcTimeout);
    m_freqCalcTimer->setInterval(1000);

    addToolBar(Qt::RightToolBarArea, ui->toolBar);//Перемещаем тулбарнаправо
}

MainWindow::~MainWindow(){
    m_serialSetting->saveOneSettings(m_conSettings);
    delete ui;
}

//*****************************************************************
//Создаем новый девайс!
void MainWindow::createRedwillDevice(ClientType type){
    if (m_dev != nullptr)
        delete m_dev;

    if(type == ClientType::COM)
        m_dev = new RedwillDevice(this, m_conSettings.host,m_conSettings.baud);
    else if(type == ClientType::TCP)
        m_dev = new RedwillDevice(this,m_conSettings.host);

    connect(m_dev,&RedwillDevice::errorOccured, this,&MainWindow::setMessage);                                          //Сообщение об ошибке
    connect(m_dev,&RedwillDevice::errorCommunication, this,[=](int,const QString &msg){setMessage(msg);});              //Сообщение об ошибке
    connect(m_dev,&RedwillDevice::errorCommunication, this,[=](int serv,const QString){communicationFailed(serv);});    //Обработка ошибки
    connect(m_dev,&RedwillDevice::modbusRequestSent, this,[=](int,const QString &msg){setMessage(msg);});               //Сообщение об отправке запроса

    connect(m_dev, &RedwillDevice::connectionStatus,this,&MainWindow::connectionChanged);                               //Состояние соединения изменилось
    connect(m_dev,&RedwillDevice::modbusDataReceved, this,&MainWindow::deviceReceived);
    connect(m_dev,&RedwillDevice::modbusAckReceved, this,&MainWindow::deviceAck);
}

//*****************************************************************
//Заполняем информацию о запрашиваемом устройстве
void MainWindow::fillDevInfo(){
    m_devInfo.type = DeviceType::ZASI;
    m_devInfo.typeStr = "ZASI";
    m_devInfo.model = m_conSettings.model;
    m_devInfo.server = m_conSettings.server;
    m_devInfo.start3adress = 0;
    m_devInfo.count3regs = 17;
    m_devInfo.modbusRegsIn.resize(17);
    m_devInfo.modbusRegsOut.resize(17);
    m_statusBar->setConText(m_conSettings.type + " on " + m_conSettings.host);
}

//*****************************************************************
//Нажата кнопка Ок в настройках. Надо полностью обновить девайсы и лукеры
void MainWindow::newSettingsOk(QList<ConSettings> &settings){
    if(settings.isEmpty())  {
        QMessageBox::critical(this,"Ошибка параметров подключения", "Произошла ошибка передачи параметров подключения. Повторите попытку!");
        return;
    }
    if(settings.at(0).type!="ZASI"){
        QMessageBox::critical(this,"Ошибка параметров подключения", "Выбран неверный тип прибора. Выберете ID в настройках и повторите попытку");
    }

    m_conSettings = settings.at(0);                 //Обновили структуру настроек
    createRedwillDevice(m_conSettings.interf);      //Пересоздали девайс

    fillDevInfo();                                  //Перезаполнили информацию об устройстве
    m_looker->setServer(m_conSettings.server);      //Обновили лукер
    m_looker->setModel(m_conSettings.model);        //Обновили лукер
}

//*****************************************************************
//CONNECT!
void MainWindow::on_actionconnect_triggered(bool checked){
    if(checked){
        m_dev->onConnect();
        ui->actionsettingsOn->setEnabled(false);
        startTime.start();
        m_statusBar->clearReSent();
    }
    else{
        m_dev->onDisconnect();
        ui->actionsettingsOn->setEnabled(true);
    }
}

//*****************************************************************
//Показать консоль
void MainWindow::on_actionconsoleOn_toggled(bool arg1){
    m_console->setVisible(arg1);
}

//*****************************************************************
//Показать настройки
void MainWindow::on_actionsettingsOn_triggered(){
   m_serialSetting->show();
}

//*****************************************************************
//*****************************************************************
//Состояние соединения изменилоось
void MainWindow::connectionChanged(const QString &host,int status){
    QString str="";

    if(status<0 || status>4) return;

    switch(status){
    case 0:   //Отключено
        str = "Отключено от " + host + "\n";
        m_statusBar->setConStatus(false);
        m_statusBar->enAnimation(false);
        m_timerSend->stop();
        m_freqCalcTimer->stop();
        ui->actionconnect->setChecked(false);
        ui->actionsettingsOn->setEnabled(true);
        break;
    case 1:    //Подключение
        str = "Подключение к " + host + "\n";
        break;
    case 2:     //Подключено
        str = "Подключено к " + host + "\n";
        m_statusBar->setConStatus(true);
        m_timerSend->start();
        m_freqCalcTimer->start();
        ui->actionconnect->setChecked(true);
        ui->actionsettingsOn->setEnabled(false);
        break;
    case 3:       //Отключение
        str = "Отключение от " + host + "\n";
        break;
    }

    m_looker->switchState(static_cast <ConectionState>(status));
    m_statusBar->setMessageBar(str);
    m_console->putData(str.toUtf8());
}

//*****************************************************************
//Добавить в очередь запросов
void MainWindow::add6ToQueue(int server,const QVector<unsigned short> & par, int startAdd, int count){
    if(m_devInfo.server == server){              //Ненужная проверка на всякий случай, которая будет нужна для >1 девайса
        for(int i=0;i<count;i++)
            m_devInfo.modbusRegsOut.replace(startAdd+i,par.at(i));
        m_devInfo.count6regs = count;
        m_devInfo.start6adress = startAdd;
        m_devInfo.func = 6;
    }
}

//*****************************************************************
//Отправляем запрос. 6, если он выставлен или 3 в противном случае.
//Тут надо подумать, если не выставлен 0, стоит ли вообще отправлять что-то или надо ждать?
void MainWindow::sendTimeout(){
    if(m_dev->isBusy())
        return;

    m_dev->modbusAct(&m_devInfo);
}

void MainWindow::freqCalcTimeout(){
    m_statusBar->setDownloadBarValue(packetCounter);
    m_statusBar->setTime(QTime(0,0).addMSecs(startTime.elapsed()));
    packetCounter=0;
}

//*****************************************************************
//Получены данные от девайса
void MainWindow::deviceReceived(int server){
    if(server == m_devInfo.server){
        QString str = "Data received from " + m_devInfo.typeStr + QString::number(m_devInfo.model) + " (SERV:" + QString::number(m_devInfo.server) + ")";
        m_statusBar->setMessageBar(str);
        m_statusBar->enAnimation(true);
        m_looker->setData(m_devInfo.modbusRegsIn);
        packetCounter++;
    }
}

//*****************************************************************
//Получено подтверждение от 6 и 16
void MainWindow::deviceAck(int server){
    if(server == m_devInfo.server)
        m_looker->switchState(ConectionState::CONNECTED);

}

//*****************************************************************
// Показать сообщение в статусе и консоли
void MainWindow::setMessage(const QString &msg){
    m_statusBar->setMessageBar(msg);
    m_console->putData(msg.toUtf8());
}

//*****************************************************************
//Ошибка в логике обмена данными (высокий уровень)
void MainWindow::communicationFailed(int serv){
    if(m_looker->server() == serv){
        m_looker->switchState(ConectionState::UNCONNECTED);
        m_looker->setEnabled(false);
        m_statusBar->incReSent();
        m_statusBar->enAnimation(false);
    }
}

//*****************************************************************
//ВКЛ/ВЫКЛ логгирование
void MainWindow::on_actionlogOn_toggled(bool arg1){
    m_looker->setLog(arg1);
}
