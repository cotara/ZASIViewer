#include "ldmdevice.h"




LDMDevice::LDMDevice(QWidget *parent, const QString & ipAdd, int port, int server, bool type,int model,int deviceType)
    : QWidget(parent), m_ipAdd_comp(ipAdd),m_port_boud(port),m_server(server),m_type(type),m_model(model){
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&LDMDevice::handlerTimer);

    if(deviceType == LDM){
        m_looker = new diameterLooker(this,m_model,m_server);
    }
    VLayout = new QVBoxLayout(this);
    VLayout->addWidget(m_looker);
}

LDMDevice::LDMDevice(QWidget *parent,QModbusClient* modbusClient,int server, bool type,int model,int deviceType)
    : QWidget(parent), m_ModbusClient(modbusClient), m_server(server),m_type(type),m_model(model){
    if(!type){
        m_port_boud = m_ModbusClient->connectionParameter(QModbusDevice::SerialBaudRateParameter).toInt();
        m_ipAdd_comp = m_ModbusClient->connectionParameter(QModbusDevice::SerialPortNameParameter).toInt();
    }
    else{
        m_port_boud = m_ModbusClient->connectionParameter(QModbusDevice::NetworkPortParameter).toInt();
        m_ipAdd_comp = m_ModbusClient->connectionParameter(QModbusDevice::NetworkAddressParameter).toInt();
    }
    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&LDMDevice::handlerTimer);
    if(deviceType == LDM){
        m_looker = new diameterLooker(this,m_model,m_server);
    }
    VLayout = new QVBoxLayout(this);
    VLayout->addWidget(m_looker);
}

LDMDevice::~LDMDevice()
{
    if (m_ModbusClient)
        m_ModbusClient->disconnectDevice();
    delete m_ModbusClient;
    delete m_timer;
}

QModbusClient *LDMDevice::getModbusClient(){
    return m_ModbusClient;
}

//Изменен тип подключениея COM-0/TCP-1
void LDMDevice::connectionTypeChanged(bool type){
    m_ModbusClient->disconnectDevice();
    delete m_ModbusClient;

    m_type = type;                                                      //Сохраняем тип текущего соединения
    if (m_type == Serial)
        m_ModbusClient = new QModbusRtuSerialMaster(this);
    else if (m_type == Tcp)
        m_ModbusClient = new QModbusTcpClient(this);


    if(!m_ModbusClient){//Если память не выделилась
        if (m_type == Serial)
            emit errorOccured("Could not create QModbusRtuSerialMaster");

        else if (m_type == Tcp)
            emit errorOccured("Could not create QModbusTcpClient");
        m_looker->setEnabled(false);
    }
    else{
        connect(m_ModbusClient, &QModbusClient::stateChanged,this,&LDMDevice::onModbusStateChanged);
        connect(m_ModbusClient, &QModbusClient::errorOccurred, this,[=]{
            emit errorOccured("Устройство " + QString::number(m_server) + " : " + m_ModbusClient->errorString());
            m_looker->setEnabled(false);
        });
    }
}


//Подключение
void LDMDevice::onConnect(){
    if(!m_ModbusClient){
         emit errorOccured("Устройство с адресом " + QString::number(m_server) + "не существует");
         m_looker->setEnabled(false);
         return;
    }

    if (m_ModbusClient->state() != QModbusDevice::ConnectedState) {
        if (m_type == Serial) {
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter,m_ipAdd_comp);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_port_boud);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        }
        else{
            m_ModbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_port_boud);
            m_ModbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, m_ipAdd_comp);
        }
        m_ModbusClient->setTimeout(100);
        m_ModbusClient->setNumberOfRetries(2);
        m_ModbusClient->connectDevice();
    }
}

void LDMDevice::onDisconnect(){
    if (m_ModbusClient->state() == QModbusDevice::ConnectedState)
        m_ModbusClient->disconnectDevice();
}
//Состояние изменилось
void LDMDevice::onModbusStateChanged(int state){
    switch(state){
        case QModbusDevice::UnconnectedState:   //Отключено
            emit connectionStatus(m_server,0,m_ipAdd_comp + ":" +QString::number(m_port_boud));
            m_timer->stop();
            break;
        case QModbusDevice::ConnectingState:    //Подключение
            emit connectionStatus(m_server,1,m_ipAdd_comp + ":" +QString::number(m_port_boud));break;
        case QModbusDevice::ConnectedState:     //Подключено
            emit connectionStatus(m_server,2,m_ipAdd_comp + ":" +QString::number(m_port_boud));
            m_timer->start();
            break;
        case QModbusDevice::ClosingState:       //Отключение
            emit connectionStatus(m_server,3,m_ipAdd_comp + ":" +QString::number(m_port_boud));break;
    }
}
//Время для отправки запроса
void LDMDevice::handlerTimer(){

    if (!m_ModbusClient){
        emit errorOccured("Устройство с адресом " + QString::number(m_server) + "не существует");
         m_looker->setEnabled(false);
        return;
    }

    QModbusDataUnit m_unit = readRequest();
    if (auto *reply = m_ModbusClient->sendReadRequest(m_unit, m_server)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &LDMDevice::onReadReady);
            emit modbusRequestSent(reply->serverAddress(),"REQ TO " +  QString::number(m_server) + " DEV: ");//Вывод в консоль
        }
        else
            delete reply; // broadcast replies return immediately
    }
    else{
        emit errorOccured("Устройство " + QString::number(reply->serverAddress()) + " : " + "Read error: " + m_ModbusClient->errorString());
        m_looker->setEnabled(false);
    }
}

//Пришел овтет
void LDMDevice::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());//ответ

    int servAdd = reply->serverAddress();               //Устройства откуда ответ
    if (!reply){
        m_looker->setEnabled(false);
        return;
    }
    if (reply->error() == QModbusDevice::NoError) {
        m_looker->setEnabled(true);
        const QModbusDataUnit unit = reply->result();
        modbusRegs.clear();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i)
            modbusRegs.append(unit.value(i));

        modbusDataProcessing();
    }
    else{
         emit errorOccured("Read response error (" + QString::number(servAdd) + " DEV): "+ reply->errorString());
         m_looker->setEnabled(false);
    }

    reply->deleteLater();
}

//Обработчки данных, пришедших от устройства
void LDMDevice::modbusDataProcessing(){
    QString str = "REPLY FROM " +  QString::number(m_server) + " DEV: ";

    for(unsigned short i:modbusRegs)
        str+=QString::number(i) + " ";
    str+="\n";

    modbusDataReceved(m_server, str);

    QVector<double> doubleData, tempData;

    if(modbusRegs.size()!=0){
        setLookerEnabled(true);
        doubleData.append((modbusRegs.at(1)+65536*modbusRegs.at(0))/1000.0);
        doubleData.append((modbusRegs.at(3)+65536*modbusRegs.at(2))/1000.0);
        doubleData.append((modbusRegs.at(5)+65536*modbusRegs.at(4))/1000.0);
        doubleData.append((60000-modbusRegs.at(9)-65536*modbusRegs.at(8))/1000.0);
        doubleData.append((60000-modbusRegs.at(11)-65536*modbusRegs.at(10))/1000.0);
        doubleData.append((modbusRegs.at(13)+65536*modbusRegs.at(12)));
        m_looker->setData(doubleData);
    }
    else{
        setLookerEnabled(false);
    }
}

QModbusDataUnit LDMDevice::readRequest() const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = 16;

    quint16 numberOfEntries = 22;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

void LDMDevice::setServer(int serverAdd){
    m_server = serverAdd;
    m_looker->setName(serverAdd);//меняем заголовок в лукере
}

void LDMDevice::setIpAdd_comp(const QString &ipAdd_comp){
    m_ipAdd_comp = ipAdd_comp;
}

void LDMDevice::setPort_boud(int port_boud){
    m_port_boud = port_boud;
}

void LDMDevice::setModel(int model){
    m_model = model;
    m_looker->setModel(model);
}

void LDMDevice::setLookerEnabled(bool state){
    m_looker->setEnabled(state);
}
