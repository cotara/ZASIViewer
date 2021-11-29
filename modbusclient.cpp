#include "modbusclient.h"
#include <QVariant>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>

enum ModbusConnection {
    Serial,
    Tcp
};

ModBusClient::ModBusClient(QObject *parent) : QObject(parent){

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&ModBusClient::handlerTimer);

    onConnectTypeChanged(false);//Включаем по умолчанию COM
}

ModBusClient::~ModBusClient()
{
    if (m_ModbusClient1)
        m_ModbusClient1->disconnectDevice();
    delete m_ModbusClient1;
    if (m_ModbusClient2)
        m_ModbusClient2->disconnectDevice();
    delete m_ModbusClient2;

    delete m_timer;
}

//Изменен тип подключение COM-0/TCP-1
void ModBusClient::onConnectTypeChanged(bool type_tcp){
    if (m_ModbusClient1) {
        m_ModbusClient1->disconnectDevice();
        delete m_ModbusClient1;
        m_ModbusClient1 = nullptr;
    }
    if (m_ModbusClient2) {
        m_ModbusClient2->disconnectDevice();
        delete m_ModbusClient2;
        m_ModbusClient2 = nullptr;
    }

    m_ModbusClient = nullptr;

    auto type = static_cast<ModbusConnection>(type_tcp);
    if (type == Serial){
        m_ModbusClient1 = new QModbusRtuSerialMaster(this);
    }
    else if (type == Tcp){
        m_ModbusClient1 = new QModbusTcpClient(this);
        m_ModbusClient2 = new QModbusTcpClient(this);
    }

    if (!m_ModbusClient1) {
        if (type == Serial)
            emit errorOccured(m_server1,"Could not create Modbus master 1.");
        else
            emit errorOccured(m_server1,"Could not create Modbus client 1.");
    }
    else{
        connect(m_ModbusClient1, &QModbusClient::stateChanged, this,[=](QModbusDevice::State state){ onModbusStateChanged(1,m_server1,state);});
        connect(m_ModbusClient1, &QModbusClient::errorOccurred, this,[=]{emit errorOccured(m_server1, m_ModbusClient1->errorString());});
    }

    if (!m_ModbusClient2 ) {
        if(type == Tcp)    //т.к. для serial второй клиент никогда не будет создан
            emit errorOccured(m_server2,"Could not create Modbus client 2.");
    }
    else{
        connect(m_ModbusClient2, &QModbusClient::stateChanged, this, [=](QModbusDevice::State state){ onModbusStateChanged(1,m_server2,state);});
        connect(m_ModbusClient2, &QModbusClient::errorOccurred, this,[=]{emit errorOccured(m_server2, m_ModbusClient2->errorString());});
    }
}

void ModBusClient::setServer1(int serverAdd){
    m_server1 = serverAdd;
}

void ModBusClient::setServer2(int serverAdd){
    m_server2 = serverAdd;
}

void ModBusClient::setDoubleMode(bool doubleMode){
    m_doubleMode = doubleMode;
}

//Подключение
//ipadd - адрес в случае с tcp или имя COM-порта в случае с serial
//port - порт в случае с tcp или скорость в случае с serial
//numDev - номер деваяйса по порядку - 1 или 2.
void ModBusClient::onConnect(int numDev, bool type, QString ipadd,int port, int server){
    if(numDev == 1){
        m_ModbusClient = m_ModbusClient1;
    }
    else if(numDev == 2){
        m_ModbusClient = m_ModbusClient2;
    }

    if (!m_ModbusClient)
        return;
    m_type = type;//Сохраняем тип текущего соединения
    if (m_ModbusClient->state() != QModbusDevice::ConnectedState) {
        if (type == Serial) {
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
        if(numDev == 1){
            m_host1 = ipadd + ":" + QString::number(port);
            m_server1 = server;
        }
        else if(numDev == 2){
            m_host2 = ipadd + ":" + QString::number(port);
            m_server2 = server;
        }

        if ( m_ModbusClient->state() != QModbusDevice::ConnectedState) {
            m_ModbusClient->setTimeout(100);
            m_ModbusClient->setNumberOfRetries(2);
            m_ModbusClient->connectDevice();
        }
    }
    else
        m_ModbusClient->disconnectDevice();
}
//Состояние изменилось
//numDev - номер устройства 1 или 2
//server - адрес устройства модбас
void ModBusClient::onModbusStateChanged(int numDev,int server, int state)
{
    QString m_host;
    if(numDev == 1)
        m_host = m_host1;
    else if(numDev == 2)
        m_host = m_host2;

    switch(state){
        case QModbusDevice::UnconnectedState:   //Отключено
            emit connectionStatus(server,0,m_host);
            m_timer->stop();
            break;
        case QModbusDevice::ConnectingState:    //Подключение
            emit connectionStatus(server,1,m_host);break;
        case QModbusDevice::ConnectedState:     //Подключено
            emit connectionStatus(server,2,m_host);
            m_timer->start();
            break;
        case QModbusDevice::ClosingState:       //Отключение
            emit connectionStatus(server,3,m_host);break;
    }
}
//Время для отправки запроса
void ModBusClient::handlerTimer(){
    //Запрос на первое устройство. Идентичен для serial и TCP
    if (!m_ModbusClient1)
        return;
    //Запрос на первое устройство
    QModbusDataUnit m_unit = readRequest();
    if (auto *reply1 = m_ModbusClient1->sendReadRequest(m_unit, m_server1)) {

        if (!reply1->isFinished()){
            connect(reply1, &QModbusReply::finished, this, &ModBusClient::onReadReady);
            emit modbusRequestSent(reply1->serverAddress(),"");//Вывод в консоль
        }
        else
            delete reply1; // broadcast replies return immediately
    }
    else
        emit errorOccured(reply1->serverAddress(), "Read error: " + m_ModbusClient1->errorString());

    if(m_doubleMode){//Если включен режим с двумя устройствами, то отправляем на второе
        if(m_type == Serial){
             if (auto *reply2 = m_ModbusClient1->sendReadRequest(readRequest(), m_server2)) {
                if (!reply2->isFinished()){
                    connect(reply2, &QModbusReply::finished, this, &ModBusClient::onReadReady);
                    emit modbusRequestSent(reply2->serverAddress(),"");//Вывод в консоль
                }
                else
                    delete reply2; // broadcast replies return immediately
            }
            else
                emit errorOccured(reply2->serverAddress(), "Read error: " + m_ModbusClient1->errorString());
        }
        else{//tcp
            if(m_ModbusClient2->state() == QModbusDevice::ConnectedState){//Если второе устройство подключено
                if (auto *reply2 = m_ModbusClient2->sendReadRequest(readRequest(), m_server2)) {
                    if (!reply2->isFinished()){
                        connect(reply2, &QModbusReply::finished, this, &ModBusClient::onReadReady);
                        emit modbusRequestSent(reply2->serverAddress(),"");//Вывод в консоль
                    }
                    else
                        delete reply2; // broadcast replies return immediately
                }
                else
                    emit errorOccured(reply2->serverAddress(), "Read error: " + m_ModbusClient2->errorString());
            }
        }
    }//режим с двумя устрйоствами

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
         emit errorOccured(servAdd, "Read response error (" + QString::number(servAdd) + " DEV): "+ reply->errorString());

    reply->deleteLater();
}

QModbusDataUnit ModBusClient::readRequest() const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = 16;

    quint16 numberOfEntries = 22;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
