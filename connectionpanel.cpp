#include "connectionpanel.h"
#include "ui_connectionpanel.h"




ConnectionPanel::ConnectionPanel(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::ConnectionPanel)
{
    setTitle("Настройки подключения");
    setObjectName("historysettings");
    ui->setupUi(this);
    ui->m_switch->setStyleSheet("QPushButton{background: transparent;}");//Прозрачная кнопка
    bool checked = false;
    ui->IPlabel->setVisible(checked);
    ui->IPlabel2->setVisible(checked);
    ui->ipAdd->setVisible(checked);
    ui->ipAdd2->setVisible(checked);
    ui->portLabel->setVisible(checked);
    ui->portLabel2->setVisible(checked);
    ui->port->setVisible(checked);
    ui->port2->setVisible(checked);

    ui->Portslabel->setVisible(!checked);
    ui->Portslabel2->setVisible(!checked);
    ui->portsBox->setVisible(!checked);
    ui->portsBox2->setVisible(!checked);
    ui->updAvblPortsButt->setVisible(!checked);
    ui->updAvblPortsButt2->setVisible(!checked);
    ui->dscrLabel->setVisible(!checked);
    ui->dscrLabel2->setVisible(!checked);
    ui->SpdLabel->setVisible(!checked);
    ui->spdBox->setVisible(!checked);


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


    //Компорты
    on_updAvblPortsButt_clicked();
    on_updAvblPortsButt2_clicked();
    ui->spdBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->spdBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->spdBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->spdBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->spdBox->addItem(QStringLiteral("230400"), 230400);
    ui->spdBox->addItem(QStringLiteral("460800"), 460800);
    ui->spdBox->addItem(QStringLiteral("921600"), 921600);

    ui->spdBox->setCurrentIndex(0);
}

ConnectionPanel::~ConnectionPanel(){
    delete ui;
}

QString ConnectionPanel::getPortName(int number){
    if(number==1)
        return ui->portsBox->currentText();
    else if(number==2)
        return ui->portsBox2->currentText();
    else
        return QString("Empty Num");
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

void ConnectionPanel::connectionChanged(int state,const QString)
{
    switch(state){
        case 0:   //Отключено
            ui->connectButton->setText("Подключиться");
            ui->connectButton->setEnabled(true);
            break;
        case 1:    //Подключение
          ui->connectButton->setEnabled(false);
        break;
        case 2:     //Подключено
            ui->connectButton->setText("Отключиться");
            ui->connectButton->setEnabled(true);
            break;
        case 3:       //Отключение
           ui->connectButton->setEnabled(false);
        break;
    }
}



//Переключение режима TCP - RS485
void ConnectionPanel::on_m_switch_clicked(bool checked){

        ui->IPlabel->setVisible(checked);
        ui->IPlabel2->setVisible(checked);
        ui->ipAdd->setVisible(checked);
        ui->ipAdd2->setVisible(checked);
        ui->portLabel->setVisible(checked);
        ui->portLabel2->setVisible(checked);
        ui->port->setVisible(checked);
        ui->port2->setVisible(checked);

        ui->Portslabel->setVisible(!checked);
        ui->Portslabel2->setVisible(!checked);
        ui->portsBox->setVisible(!checked);
        ui->portsBox2->setVisible(!checked);
        ui->updAvblPortsButt->setVisible(!checked);
        ui->updAvblPortsButt2->setVisible(!checked);
        ui->dscrLabel->setVisible(!checked);
        ui->dscrLabel2->setVisible(!checked);
        ui->SpdLabel->setVisible(!checked);
        ui->spdBox->setVisible(!checked);


        emit connectionTypeChanged(checked);
        on_oneTwoCheck_stateChanged(ui->oneTwoCheck->checkState());

}

//Выбор ком-порта
void ConnectionPanel::on_portsBox_currentIndexChanged(int i){

    if (i != -1) {
        QSerialPortInfo serial_info;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int mun = ports.count();
        if (mun-1<i){
                QMessageBox::critical(this,"Внимание!","COM-порт 1 отсутсвует");
                on_updAvblPortsButt_clicked();
                return;
        }
        serial_info=ports.takeAt(i);

        ui->dscrLabel->setText(serial_info.description());

    }
}
void ConnectionPanel::on_portsBox2_currentIndexChanged(int i)
{
    if (i != -1) {
        QSerialPortInfo serial_info;
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        int mun = ports.count();
        if (mun-1<i){
                QMessageBox::critical(this,"Внимание!","COM-порт 2 отсутсвует");
                on_updAvblPortsButt2_clicked();
                return;
        }
        serial_info=ports.takeAt(i);

        ui->dscrLabel2->setText(serial_info.description());

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

void ConnectionPanel::on_updAvblPortsButt2_clicked()
{
    ui->portsBox2->clear();
    QStringList list;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    if (ports.isEmpty())
        QMessageBox::warning(this,"Внимание!","В системе нет ни одного COM-порта");
    for (QSerialPortInfo port : ports) {
        list << port.portName();
     }
     ui->portsBox2->addItems(list);
}


//ПОДКЛЮЧИТЬСЯ!
void ConnectionPanel::on_connectButton_clicked(){
    if(!ui->m_switch->isChecked()){//Выбран режим компорта
       emit connectionPushed(1, false,ui->portsBox->currentText(),ui->spdBox->currentText().toInt(),ui->deviceNumLine->text().toInt());
       //if(ui->oneTwoCheck->isChecked())
        //    emit connectionPushed(2, false,ui->portsBox2->currentText(),ui->spdBox->currentText().toInt(),ui->deviceNumLine2->text().toInt());
    }
    else{//Выбран режим TCP|IP
        emit connectionPushed(1, true,getIpAdd(1),getPort(1),ui->deviceNumLine->text().toInt());
        if(ui->oneTwoCheck->isChecked())
            emit connectionPushed(2, true,getIpAdd(2),getPort(2),ui->deviceNumLine2->text().toInt());
    }
}


void ConnectionPanel::on_oneTwoCheck_stateChanged(int arg1){
    if(!ui->m_switch->isChecked()){
        ui->Portslabel->setVisible(true);
        ui->Portslabel2->setVisible(arg1);
        ui->portsBox->setVisible(true);
        ui->portsBox2->setVisible(arg1);
        ui->updAvblPortsButt->setVisible(true);
        ui->updAvblPortsButt2->setVisible(arg1);
        ui->dscrLabel->setVisible(true);
        ui->dscrLabel2->setVisible(arg1);
        ui->SpdLabel->setVisible(true);
        ui->spdBox->setVisible(true);
    }
    else{
        ui->IPlabel->setVisible(true);
        ui->IPlabel2->setVisible(arg1);
        ui->ipAdd->setVisible(true);
        ui->ipAdd2->setVisible(arg1);
        ui->portLabel->setVisible(true);
        ui->portLabel2->setVisible(arg1);
        ui->port->setVisible(true);
        ui->port2->setVisible(arg1);
    }
}

