#include "modbusclient.h"
#include <QVariant>
#include <QModbusRtuSerialMaster>
#include <QSerialPort>

enum ModbusConnection {
    Serial,
    Tcp
};


ModBusClient::ModBusClient(QObject *parent) : QObject(parent){

    m_timer1 = new QTimer(this);
    m_timer1->setInterval(1000);
    connect(m_timer1,&QTimer::timeout,this,&ModBusClient::handlerTimer1);

    m_timer2 = new QTimer(this);
    m_timer2->setInterval(1000);
    connect(m_timer2,&QTimer::timeout,this,&ModBusClient::handlerTimer2);

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

    delete m_timer1;
    delete m_timer2;
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
        m_ModbusClient2 = new QModbusRtuSerialMaster(this);
    }
    else if (type == Tcp){
        m_ModbusClient1 = new QModbusTcpClient(this);
        m_ModbusClient2 = new QModbusTcpClient(this);
    }

    if (!m_ModbusClient1) {
        if (type == Serial)
            emit errorOccured("Could not create Modbus master 1.");
        else
            emit errorOccured("Could not create Modbus client 1.");
    }
    else{
        connect(m_ModbusClient1, &QModbusClient::stateChanged, this,[=](QModbusDevice::State state){ onModbusStateChanged(1,state);});
        connect(m_ModbusClient1, &QModbusClient::errorOccurred, this,[=]{emit errorOccured(m_ModbusClient1->errorString());});
    }
    if (!m_ModbusClient2) {
        if (type == Serial)
            emit errorOccured("Could not create Modbus master 2.");
        else
            emit errorOccured("Could not create Modbus client 2.");
    }
    else{
        connect(m_ModbusClient2, &QModbusClient::stateChanged, this, [=](QModbusDevice::State state){ onModbusStateChanged(2,state);});
        connect(m_ModbusClient2, &QModbusClient::errorOccurred, this,[=]{emit errorOccured(m_ModbusClient2->errorString());});
    }
}

//Подключение
//ipadd - адрес в случае с tcp или имя COM-порта в случае с serial
//port - порт в случае с tcp или скорость в случае с serial
void ModBusClient::onConnect(int numDev, bool type, QString ipadd,int port, int server){
    if(numDev == 1){
        m_ModbusClient = m_ModbusClient1;
    }
    else if(numDev == 2){
        m_ModbusClient = m_ModbusClient2;
    }

    if (!m_ModbusClient)
        return;
    QModbusDevice::State state = m_ModbusClient->state();
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

        QModbusDevice::State m_state = m_ModbusClient->state();
        if ( m_state != QModbusDevice::ConnectedState) {
            m_ModbusClient->setTimeout(1000);
            m_ModbusClient->setNumberOfRetries(5);
            m_ModbusClient->connectDevice();
        }
    }
    else
        m_ModbusClient->disconnectDevice();
}
//Состояние изменилось
void ModBusClient::onModbusStateChanged(int numDev, int state)
{
    QString m_host;
    QTimer *m_timer;
    if(numDev == 1){
        m_host = m_host1;
        m_timer = m_timer1;
    }
    else if(numDev == 2){
        m_host = m_host2;
        m_timer = m_timer2;
    }
    switch(state){
        case QModbusDevice::UnconnectedState:   //Отключено
            emit connectionStatus(0,m_host);
            m_timer->stop();
            break;
        case QModbusDevice::ConnectingState:    //Подключение
            emit connectionStatus(1,m_host);break;
        case QModbusDevice::ConnectedState:     //Подключено
            emit connectionStatus(2,m_host);
            m_timer->start();
            break;
        case QModbusDevice::ClosingState:       //Отключение
            emit connectionStatus(3,m_host);break;
    }
}
//Время для отправки запроса
void ModBusClient::handlerTimer1(){
    //Отправляем запрос на все регистры раз в секунду
    if (!m_ModbusClient1)
        return;
    if (auto *reply = m_ModbusClient1->sendReadRequest(readRequest(), m_server1)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModBusClient::onReadReady1);
        else
            delete reply; // broadcast replies return immediately
    }
    else
        emit errorOccured("Read error: " + m_ModbusClient1->errorString());

}
void ModBusClient::handlerTimer2(){
    //Отправляем запрос на все регистры раз в секунду
    if (!m_ModbusClient2)
        return;
    if (auto *reply = m_ModbusClient2->sendReadRequest(readRequest(), m_server2)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &ModBusClient::onReadReady2);
        else
            delete reply; // broadcast replies return immediately
    }
    else
        emit errorOccured("Read error: " + m_ModbusClient2->errorString());

}


//Пришел овтет
void ModBusClient::onReadReady1()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        modbusRegs1.clear();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
            //const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i).arg(QString::number(unit.value(i), unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            modbusRegs1.append(unit.value(i));
            //ui->readValue->addItem(entry);
        }
        emit modbusDataReceved(1,modbusRegs1);
    }
    else if (reply->error() == QModbusDevice::ProtocolError) {
        emit errorOccured("Read response error (1dev): " + reply->errorString());
        /*statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);*/
    }
    else {
        emit errorOccured("Read response error (1dev): " + reply->errorString());
        /*statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);*/
    }

    reply->deleteLater();
}
//Пришел овтет
void ModBusClient::onReadReady2()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        modbusRegs2.clear();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
            //const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i).arg(QString::number(unit.value(i), unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            modbusRegs2.append(unit.value(i));
            //ui->readValue->addItem(entry);
        }
        emit modbusDataReceved(2,modbusRegs2);
    }
    else if (reply->error() == QModbusDevice::ProtocolError) {
        emit errorOccured("Read response error: (2dev)" + reply->errorString());
        /*statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);*/
    }
    else {
        emit errorOccured("Read response error: (2dev)" + reply->errorString());
        /*statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);*/
    }

    reply->deleteLater();
}
QModbusDataUnit ModBusClient::readRequest() const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = 16;

    quint16 numberOfEntries = 22;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
