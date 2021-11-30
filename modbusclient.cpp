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
    if (m_ModbusClient1)
        m_ModbusClient1->disconnectDevice();
    delete m_ModbusClient1;
    if (m_ModbusClient2)
        m_ModbusClient2->disconnectDevice();
    delete m_ModbusClient2;

    delete m_timer;
}

//Изменен тип подключениея COM-0/TCP-1
void ModBusClient::onConnectTypeChanged(bool type_tcp){

    for(QModbusClient *i:m_ModbusClients){
        i->disconnectDevice();
        delete i;
    }
    m_ModbusClients.clear();
    m_type = type_tcp;              //Сохраняем тип текущего соединения
    for(int i=0;i<m_doubleMode+1;i++){
        if (type_tcp == Serial)
            m_ModbusClients.append(new QModbusRtuSerialMaster(this));
        else if (type_tcp == Tcp)
            m_ModbusClients.append(new QModbusTcpClient(this));

        if (!m_ModbusClients[i]) {
            if (type_tcp == Serial)
                emit errorOccured(m_servers[i],"Could not create Modbus RTU master #" + QString::number(i));
             else if (type_tcp == Tcp)
                emit errorOccured(m_servers[i],"Could not create Modbus TCP client"+ QString::number(i));
        }
        else{
            connect(m_ModbusClients[i], &QModbusClient::stateChanged, this,[=](QModbusDevice::State state){ onModbusStateChanged(i,m_servers[i],state);});
            connect(m_ModbusClients[i], &QModbusClient::errorOccurred, this,[=]{emit errorOccured(m_servers[i], m_ModbusClients[i]->errorString());});
        }
    }
}

void ModBusClient::setServer(int numDev, int serverAdd){
    if(m_servers.size()<numDev+1)
        m_servers.append(serverAdd);
    else
        m_servers[numDev] =serverAdd;
}


void ModBusClient::setDoubleMode(bool doubleMode){
    m_doubleMode = doubleMode;
}

//Подключение
//ipadd - адрес в случае с tcp или имя COM-порта в случае с serial
//port - порт в случае с tcp или скорость в случае с serial
//numDev - номер деваяйса по порядку - 1 или 2.
void ModBusClient::onConnect(int numDev, QString ipadd,int port, int server){
    if(m_ModbusClients.size()< numDev+1){
         emit errorOccured(server,"Could not connect to " + ipadd + ":" + QString::number(port));
    }

    if (m_ModbusClients[numDev]->state() != QModbusDevice::ConnectedState) {
        if (m_type == Serial) {
                m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::SerialPortNameParameter,ipadd);
                m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, port);
                m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::SerialParityParameter,QSerialPort::NoParity);
                m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
                m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);
        }
        else{
            m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::NetworkPortParameter, port);
            m_ModbusClients[numDev]->setConnectionParameter(QModbusDevice::NetworkAddressParameter, ipadd);
        }
        m_hosts[numDev] = ipadd + ":" + QString::number(port);
        m_servers[numDev] = server;

        m_ModbusClients[numDev]->setTimeout(100);
        m_ModbusClients[numDev]->setNumberOfRetries(2);
        m_ModbusClients[numDev]->connectDevice();

    }
    else
        m_ModbusClients[numDev]->disconnectDevice();
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
