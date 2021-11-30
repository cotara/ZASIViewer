#include "modbusclient.h"
#include <QVariant>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>

enum ConnectionType {
    Serial,
    Tcp
};

ModBusClient::ModBusClient(QObject *parent) : QObject(parent){

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&ModBusClient::handlerTimer);

    onConnectTypeChanged(Serial);//Включаем по умолчанию COM
}

ModBusClient::~ModBusClient()
{
    if (m_ModbusClient)
        m_ModbusClient->disconnectDevice();
    delete m_ModbusClient;
    delete m_timer;
}
//Изменен тип подключениея COM-0/TCP-1
void ModBusClient::onConnectTypeChanged(bool type_tcp){
    m_ModbusClient->disconnectDevice();
    delete m_ModbusClient;

    m_type = type_tcp;              //Сохраняем тип текущего соединения
    if (type_tcp == Serial)
        m_ModbusClient = new QModbusRtuSerialMaster(this);
    else if (type_tcp == Tcp)
        m_ModbusClient = new QModbusTcpClient(this);

    if(!m_ModbusClient){//Если память не выделилась
        if (type_tcp == Serial)
            emit errorOccured("Could not create Modbus RTU master #" + QString::number(m_server));
         else if (type_tcp == Tcp)
            emit errorOccured("Could not create Modbus TCP client"+ QString::number(m_server));
    }
    else{
        connect(m_ModbusClient, &QModbusClient::stateChanged,this,&ModBusClient::onModbusStateChanged);
        connect(m_ModbusClient, &QModbusClient::errorOccurred, this,[=]{emit errorOccured("Устройство " + QString::number(m_server) + " : " + m_ModbusClient->errorString());});
    }
}

void ModBusClient::setServer(int serverAdd){
    m_server = serverAdd;
}

//Подключение
//ipadd - адрес в случае с tcp или имя COM-порта в случае с serial
//port - порт в случае с tcp или скорость в случае с serial
//numDev - номер девайса по порядку - 1 или 2.
void ModBusClient::onConnect(QString ipadd,int port, int server){
    if(!m_ModbusClient)
         emit errorOccured("Устройство с адресом " + QString::number(server) + "Отключено от " + ipadd + ":" + QString::number(port));

    if (m_ModbusClient->state() != QModbusDevice::ConnectedState) {
        if (m_type == Serial) {
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter,ipadd);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, port);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
                m_ModbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        }
        else{
            m_ModbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
            m_ModbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipadd);
        }
        m_host = ipadd + ":" + QString::number(port);
        m_server = server;

        m_ModbusClient->setTimeout(100);
        m_ModbusClient->setNumberOfRetries(2);
        m_ModbusClient->connectDevice();
    }
    else
        m_ModbusClient->disconnectDevice();
}
//Состояние изменилось
//numDev - номер устройства 1 или 2
//server - адрес устройства модбас
void ModBusClient::onModbusStateChanged(int state){
    switch(state){
        case QModbusDevice::UnconnectedState:   //Отключено
            emit connectionStatus(m_server,0,m_host);
            m_timer->stop();
            break;
        case QModbusDevice::ConnectingState:    //Подключение
            emit connectionStatus(m_server,1,m_host);break;
        case QModbusDevice::ConnectedState:     //Подключено
            emit connectionStatus(m_server,2,m_host);
            m_timer->start();
            break;
        case QModbusDevice::ClosingState:       //Отключение
            emit connectionStatus(m_server,3,m_host);break;
    }
}
//Время для отправки запроса
void ModBusClient::handlerTimer(){

    if (!m_ModbusClient)
        return;

    QModbusDataUnit m_unit = readRequest();
    if (auto *reply = m_ModbusClient->sendReadRequest(m_unit, m_server)) {
        if (!reply->isFinished()){
            connect(reply, &QModbusReply::finished, this, &ModBusClient::onReadReady);
            emit modbusRequestSent(reply->serverAddress(),"");//Вывод в консоль
        }
        else
            delete reply; // broadcast replies return immediately
    }
    else
        emit errorOccured("Устройство " + QString::number(reply->serverAddress()) + " : " + "Read error: " + m_ModbusClient->errorString());
}

//Пришел овтет
void ModBusClient::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());//ответ

    int servAdd = reply->serverAddress();               //Устройства откуда ответ
    if (!reply)
        return;
    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        modbusRegs.clear();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i)
            modbusRegs.append(unit.value(i));

        emit modbusDataReceved(servAdd,modbusRegs);
    }
    else
         emit errorOccured("Read response error (" + QString::number(servAdd) + " DEV): "+ reply->errorString());

    reply->deleteLater();
}

QModbusDataUnit ModBusClient::readRequest() const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = 16;

    quint16 numberOfEntries = 22;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
