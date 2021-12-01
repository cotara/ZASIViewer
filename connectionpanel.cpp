#include "connectionpanel.h"
#include "ui_connectionpanel.h"




ConnectionPanel::ConnectionPanel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConnectionPanel)
{
    setTitle("Настройки подключения");
    setObjectName("historysettings");
    ui->setupUi(this);


    interfaceSwitch(false);//С ком-портом
    //IP
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);
    ui->ipAdd->setValidator(ipValidator);
    ui->ipAdd2->setValidator(ipValidator);
    ui->ipAdd->setText("127.0.0.1");
    ui->ipAdd2->setText("127.0.0.1");
    ui->port->setText("502");
    ui->port2->setText("502");
    ui->port->setEnabled(true);
    ui->port2->setEnabled(true);

    //Компорты
    on_updAvblPortsButt_clicked();
    ui->spdBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->spdBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->spdBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->spdBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->spdBox->addItem(QStringLiteral("230400"), 230400);
    ui->spdBox->addItem(QStringLiteral("460800"), 460800);
    ui->spdBox->addItem(QStringLiteral("921600"), 921600);

    ui->spdBox->setCurrentIndex(0);
    //Изменился сервер
    connect(ui->deviceNumLine,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit serverChanged(0,i);});
    connect(ui->deviceNumLine2,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit serverChanged(1,i);});
    //Изменилась модель
    connect(ui->diamSpinBox1,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit modelChanged(0,i);});
    connect(ui->diamSpinBox2,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit modelChanged(1,i);});
    //Изменился айпи или номер компорта
    connect(ui->ipAdd,&QLineEdit::textChanged,[=](const QString &ipAdd_comp){ emit ipAdd_compChanged(0,ipAdd_comp);});
    connect(ui->ipAdd2,&QLineEdit::textChanged,[=](const QString &ipAdd_comp){ emit ipAdd_compChanged(1,ipAdd_comp);});
    connect(ui->portsBox,&QComboBox::currentTextChanged,[=](const QString &comport){ emit ipAdd_compChanged(0,comport);});
    //Изменился порт или скорость
    connect(ui->port,&QLineEdit::textChanged,[=](const QString &port){ emit port_boudChanged(0,port.toInt());});
    connect(ui->port2,&QLineEdit::textChanged,[=](const QString &port){ emit port_boudChanged(1,port.toInt());});
    connect(ui->spdBox,&QComboBox::currentTextChanged,[=](const QString &baud){ emit port_boudChanged(0,baud.toInt());});

    ui->deviceNumLine->setValue(1);
    ui->deviceNumLine2->setValue(2);

    ui->diamSpinBox1->setValue(50);
    ui->diamSpinBox2->setValue(50);

    connect(ui->clearButton,&QPushButton::clicked,this,&ConnectionPanel::clearConsole);
}

ConnectionPanel::~ConnectionPanel(){
    delete ui;
}


QString ConnectionPanel::getIpAdd(int number){
    if(number==0)
        return ui->ipAdd->text();
    else if(number==1)
        return ui->ipAdd2->text();
    else
        return QString("Empty Num");
}

int ConnectionPanel::getPort(int number){
    if(number==0)
        return ui->port->text().toInt();
    else if(number==1)
        return ui->port2->text().toInt();
    else
        return -1;
}

int ConnectionPanel::getServer(int number){
    if(number==0)
        return ui->deviceNumLine->value();
    else if(number==1)
        return ui->deviceNumLine2->value();
    else
        return -1;
}

int ConnectionPanel::getDiam(int number)
{
    if(number==0)
        return ui->diamSpinBox1->value();
    else if(number==1)
        return ui->diamSpinBox2->value();
    else
        return -1;
}
//Изменение кнопки при изменении подключения
void ConnectionPanel::connectionChanged(int , int state,const QString)
{
    switch(state){
        case 0:   //Отключено
            ui->connectButton->setText("Подключиться");
            ui->connectButton->setEnabled(true);
            elementsDisable(true);
            break;
        case 1:    //Подключение
            ui->connectButton->setEnabled(false);
            elementsDisable(false);
            break;
        case 2:     //Подключено
            ui->connectButton->setText("Отключиться");
            ui->connectButton->setEnabled(true);
            elementsDisable(false);
            break;
        case 3:       //Отключение
           ui->connectButton->setEnabled(false);
           elementsDisable(false);
        break;
    }
}


//Выбор ком-порта
void ConnectionPanel::on_portsBox_currentIndexChanged(int index){

    if (index != -1) {
        QSerialPortInfo serial_info;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int mun = ports.count();
        if (mun-1<index){
                QMessageBox::critical(this,"Внимание!","COM-порт отсутсвует");
                on_updAvblPortsButt_clicked();
                return;
        }
        serial_info=ports.takeAt(index);
        ui->dscrLabel->setText(serial_info.description());
    }
}


//Обновить доступные компорты
void ConnectionPanel::on_updAvblPortsButt_clicked(){

    ui->portsBox->clear();
    QStringList list;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty())
        QMessageBox::warning(this,"Внимание!","В системе нет ни одного COM-порта");
    for (QSerialPortInfo port : ports) {
        list << port.portName();
     }
     ui->portsBox->addItems(list);
}

//ПОДКЛЮЧИТЬСЯ!
void ConnectionPanel::on_connectButton_clicked(){
    if(ui->connectButton->text() == "Подключиться")
        emit connectionPushed(true);

    else if(ui->connectButton->text() == "Отключиться")
        emit connectionPushed(false);

}

//Переключение режима TCP - SERIAL
void ConnectionPanel::interfaceSwitch(bool type){
        ui->IPlabel->setVisible(type);
        ui->IPlabel2->setVisible(type);
        ui->ipAdd->setVisible(type);
        ui->ipAdd2->setVisible(type);
        ui->portLabel->setVisible(type);
        ui->portLabel2->setVisible(type);
        ui->port->setVisible(type);
        ui->port2->setVisible(type);

        ui->Portslabel->setVisible(!type);
        ui->portsBox->setVisible(!type);
        ui->updAvblPortsButt->setVisible(!type);
        ui->dscrLabel->setVisible(!type);
        ui->SpdLabel->setVisible(!type);
        ui->spdBox->setVisible(!type);
}

void ConnectionPanel::setStatusLabel(int numDev, bool state){
    if (state) {
        if(numDev==0){
            ui->statusLabel->setStyleSheet(lightgreen);
            ui->statusLabel->setText("On-line");
        }
        else if(numDev==1){
            ui->statusLabel2->setStyleSheet(lightgreen);
            ui->statusLabel2->setText("On-line");
        }
    }
    else{
        if(numDev==0){
            ui->statusLabel->setStyleSheet(red);
            ui->statusLabel->setText("Off-line");
        }
        else if(numDev==1){
            ui->statusLabel2->setStyleSheet(red);
            ui->statusLabel2->setText("Off-line");
        }

    }
}
//Переключение режимов 1-2 устройства
void ConnectionPanel::oneTwoChange(int arg1){
    if(m_interface){//Если текущий режим - tcp
        ui->IPlabel2->setVisible(arg1);
        ui->ipAdd2->setVisible(arg1);
        ui->portLabel2->setVisible(arg1);
        ui->port2->setVisible(arg1);
    }

    ui->deviceNumLine2->setVisible(arg1);
    ui->diamSpinBox2->setVisible(arg1);
    ui->ldm2_label->setVisible(arg1);
}

void ConnectionPanel::elementsDisable(bool state){
    emit doubleButtonBlock(state);
    ui->portsBox->setEnabled(state);
    ui->updAvblPortsButt->setEnabled(state);
    ui->spdBox->setEnabled(state);
    ui->deviceNumLine->setEnabled(state);
    ui->deviceNumLine2->setEnabled(state);
    ui->ipAdd->setEnabled(state);
    ui->ipAdd2->setEnabled(state);
    ui->port->setEnabled(state);
    ui->port2->setEnabled(state);
}



