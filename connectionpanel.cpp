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

    connect(ui->deviceNumLine,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit server1changed(i);});
    connect(ui->deviceNumLine2,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit server2changed(i);});

    connect(ui->diamSpinBox1,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit diameter1changed(i);});
    connect(ui->diamSpinBox2,QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ emit diameter2changed(i);});

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
    if(number==1)
        return ui->ipAdd->text();
    else if(number==2)
        return ui->ipAdd2->text();
    else
        return QString("Empty Num");
}

int ConnectionPanel::getPort(int number){
    if(number==1)
        return ui->port->text().toInt();
    else if(number==2)
        return ui->port2->text().toInt();
    else
        return -1;
}

int ConnectionPanel::getServer(int number){
    if(number==1)
        return ui->deviceNumLine->value();
    else if(number==2)
        return ui->deviceNumLine2->value();
    else
        return -1;
}

int ConnectionPanel::getDiam(int number)
{
    if(number==1)
        return ui->diamSpinBox1->value();
    else if(number==2)
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
void ConnectionPanel::on_portsBox_currentIndexChanged(int i){

    if (i != -1) {
        QSerialPortInfo serial_info;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int mun = ports.count();
        if (mun-1<i){
                QMessageBox::critical(this,"Внимание!","COM-порт отсутсвует");
                on_updAvblPortsButt_clicked();
                return;
        }
        serial_info=ports.takeAt(i);

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
    if(!m_interface)//Выбран режим компорта
       emit connectionPushed(1, false,ui->portsBox->currentText(),ui->spdBox->currentText().toInt(),ui->deviceNumLine->text().toInt());

    else{//Выбран режим TCP|IP
        emit connectionPushed(1, true,getIpAdd(1),getPort(1),ui->deviceNumLine->text().toInt());
        if(m_doubleMode)
            emit connectionPushed(2, true,getIpAdd(2),getPort(2),ui->deviceNumLine2->text().toInt());
    }
}


//Переключение режима TCP - RS485
void ConnectionPanel::interfaceSwitch(bool checked){

        ui->IPlabel->setVisible(checked);
        ui->IPlabel2->setVisible(checked);
        ui->ipAdd->setVisible(checked);
        ui->ipAdd2->setVisible(checked);
        ui->portLabel->setVisible(checked);
        ui->portLabel2->setVisible(checked);
        ui->port->setVisible(checked);
        ui->port2->setVisible(checked);

        ui->Portslabel->setVisible(!checked);
        ui->portsBox->setVisible(!checked);
        ui->updAvblPortsButt->setVisible(!checked);
        ui->dscrLabel->setVisible(!checked);
        ui->SpdLabel->setVisible(!checked);
        ui->spdBox->setVisible(!checked);


        emit connectionTypeChanged(checked);
        oneTwoChange(m_doubleMode);

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



