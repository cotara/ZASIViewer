#include "ldmdevice.h"
//Конструктор по умолчанию
LDMDevice::LDMDevice(QWidget *parent, const QString & ipAdd, int port, int server, bool type,int model,int deviceType)
    : QWidget(parent), m_ipAdd_comp(ipAdd),m_port_boud(port),m_server(server),m_type(type),m_model(model){

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&LDMDevice::handlerTimer);

    reconnectionTimer =new QTimer(this);
    reconnectionTimer->setInterval(1000);
    connect(reconnectionTimer,&QTimer::timeout,this,&LDMDevice::handlerReconnectionTimer);

    if(deviceType == ZASI){
        m_looker = new ZasiLooker(this,m_model,m_server);
        connect(m_looker,&ZasiLooker::modelChanged,this,&LDMDevice::modelChanged);
    }
    createNewClien(nullptr, type);//Создаем нового клиента с коннектами

    VLayout = new QVBoxLayout(this);
    VLayout->addWidget(m_looker);
    isMaster = true;//т.к. это конструктор для главного устройства
    connect(m_looker,&ZasiLooker::onSetReg,this,&LDMDevice::setReg);

}
//Конструктор для создания девайса-копии с копией QModbusClient (для Serial) подключения
LDMDevice::LDMDevice(QWidget *parent,QModbusClient* modbusClient,int server, bool type,int model,int deviceType)
    : QWidget(parent), m_ModbusClient(modbusClient), m_server(server),m_type(type),m_model(model){
    if(!type){
        m_port_boud = m_ModbusClient->connectionParameter(QModbusDevice::SerialBaudRateParameter).toInt();
        m_ipAdd_comp = m_ModbusClient->connectionParameter(QModbusDevice::SerialPortNameParameter).toString();
    }
    else{
        m_port_boud = m_ModbusClient->connectionParameter(QModbusDevice::NetworkPortParameter).toInt();
        m_ipAdd_comp = m_ModbusClient->connectionParameter(QModbusDevice::NetworkAddressParameter).toString();
    }
    createNewClien(m_ModbusClient, type);//Дублируем клиента и коннектим к нему слоты

    m_timer = new QTimer(this);
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&LDMDevice::handlerTimer);
    if(deviceType == ZASI){
        m_looker = new ZasiLooker(this,m_model,m_server);
    }
    VLayout = new QVBoxLayout(this);
    VLayout->addWidget(m_looker);
    isMaster = false;//т.к. это конструктор для ведомого
}

LDMDevice::~LDMDevice()
{
    if(isMaster && m_ModbusClient){//Удаляем только данные по указателю мастера
        m_ModbusClient->disconnectDevice();
        delete m_ModbusClient;
        m_ModbusClient = nullptr;
    }
    delete m_timer;
    delete m_looker;
    delete VLayout;
}

QModbusClient *LDMDevice::getModbusClient(){
    return m_ModbusClient;
}

//Изменение типа устройства путем пересоздания клиента (Serial|TCP)
//Если нужно устройство со своим отдельным клиентом, как в TCP, то client должен быть равен nullptr
void LDMDevice::createNewClien(QModbusClient *client, bool type){

    m_type = type; //Сохраняем тип текущего соединения

    if(client == nullptr){//Выделяем память для нового клиента
        if (m_type == Serial){
            m_ModbusClient = new QModbusRtuSerialMaster(this);
            // Эти две строки для того, чтобы конструктор ведомого Serial смог продублировать настройки
            m_ModbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter,m_ipAdd_comp);
            m_ModbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_port_boud);
        }
        else if (m_type == Tcp)
            m_ModbusClient = new QModbusTcpClient(this);
        isMaster = true;
    }
    else{
        m_ModbusClient = client;//Иначе получаем устройство с клиентом дубликатом
        isMaster = false;
    }


    if(!m_ModbusClient){//Если память не выделилась
        if (m_type == Serial)
            emit errorOccured("Could not create QModbusRtuSerialMaster\n");
        else if (m_type == Tcp)
            emit errorOccured("Could not create QModbusTcpClient\n");
        m_looker->setEnabled(false);
    }
    else{
        connect(m_ModbusClient, &QModbusClient::stateChanged,this,&LDMDevice::onModbusStateChanged);
        connect(m_ModbusClient, &QModbusClient::errorOccurred, this,[=]{
            emit errorOccured("Устройство " + QString::number(m_server) + " : " + m_ModbusClient->errorString()+"\n");
            reconnectionTimer->start();

            m_looker->setEnabled(false);
        });
    }
}

//Подключение
void LDMDevice::onConnect(){
    if(!m_ModbusClient){
         emit errorOccured("Устройство с адресом " + QString::number(m_server) + "не существует"+"\n");
         m_looker->setEnabled(false);
         return;
    }

    if (m_ModbusClient->state() != QModbusDevice::ConnectedState && m_ModbusClient->state() != QModbusDevice::ConnectingState && m_ModbusClient->state() != QModbusDevice::ClosingState) {
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
        m_ModbusClient->setTimeout(200);
        m_ModbusClient->setNumberOfRetries(0);
        m_ModbusClient->connectDevice();
        m_looker->onConnect(true);
    }
}

void LDMDevice::onDisconnect(){
    if (m_ModbusClient->state() == QModbusDevice::ConnectedState){
        m_ModbusClient->disconnectDevice();
        m_looker->onConnect(false);
    }
}
//Состояние изменилось
void LDMDevice::onModbusStateChanged(int state){
    switch(state){
        case QModbusDevice::UnconnectedState:   //Отключено
            emit connectionStatus(m_server,0,m_ipAdd_comp + ":" +QString::number(m_port_boud));
            m_looker->setEnabled(false);
            m_timer->stop();
            break;
        case QModbusDevice::ConnectingState:    //Подключение
            emit connectionStatus(m_server,1,m_ipAdd_comp + ":" +QString::number(m_port_boud));break;
        case QModbusDevice::ConnectedState:     //Подключено
            emit connectionStatus(m_server,2,m_ipAdd_comp + ":" +QString::number(m_port_boud));
            m_looker->setEnabled(true);
            m_timer->start();
            break;
        case QModbusDevice::ClosingState:       //Отключение
            emit connectionStatus(m_server,3,m_ipAdd_comp + ":" +QString::number(m_port_boud));break;
    }
}
//Время для отправки запроса
void LDMDevice::handlerTimer(){

    if (!m_ModbusClient){
        emit errorOccured("Устройство с адресом " + QString::number(m_server) + "не существует"+"\n");
         m_looker->setEnabled(false);
        return;
    }
    QModbusDataUnit m_unit;
    //Если надо послать 6-ю, шлем. Чтобы не смешалось с 3-й
    if(flag6func){
        m_unit = writeRequest(addrToSend,counToSend);
        for (int i = 0, total = int(m_unit.valueCount()); i < total; ++i)
             m_unit.setValue(i, dataToSend.at(i));
        if (auto *reply = m_ModbusClient->sendWriteRequest(m_unit, m_server)) {
            if (!reply->isFinished()) {
                    connect(reply, &QModbusReply::finished, this, &LDMDevice::ackReady);
                    emit modbusRequestSent(reply->serverAddress(),"WRITE TO " +  QString::number(m_server) + " DEV: ");//Вывод в консоль
            }
            else
                reply->deleteLater();// broadcast replies return immediately
        }
        else
            emit errorOccured("Write error: " + m_ModbusClient->errorString()+"\n");
        flag6func=false;
    }
    else {
        m_unit = readRequest();
        if (auto *reply = m_ModbusClient->sendReadRequest(m_unit, m_server)) {
            if (!reply->isFinished()){
                connect(reply, &QModbusReply::finished, this, &LDMDevice::onReadReady);
                emit modbusRequestSent(reply->serverAddress(),"REQ TO " +  QString::number(m_server) + " DEV: ");//Вывод в консоль
            }
            else
                delete reply; // broadcast replies return immediately
        }
        else{
            emit errorOccured("Read error: " + m_ModbusClient->errorString()+"\n");
            m_looker->setEnabled(false);
        }
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
        errorsCounter=0;
    }
    else{
         emit errorOccured("Read error (" + QString::number(servAdd) + " DEV): "+ reply->errorString()+"\n");
         m_looker->setEnabled(false);
         errorsCounter++;
         if(errorsCounter>5){
             onDisconnect();//Если ответ неправильный 5 раз, пробуем переподключаться
             onConnect();
             errorsCounter=0;
         }
    }
    reply->deleteLater();
}

//Обработчки данных, пришедших от устройства
void LDMDevice::modbusDataProcessing(){
    QString str = "REPLY FROM " +  QString::number(m_server) + " DEV: ";

    for(unsigned short i:modbusRegs)
        str+=QString::number(i) + " ";
    str+="\n";

    emit modbusDataReceved(m_server, str);

    QVector<short> shortData;

    if(modbusRegs.size()!=0){
        setLookerEnabled(true);
        for(unsigned short i:modbusRegs)
            shortData.append(static_cast<short>(i));
        m_looker->setData(shortData);
    }
    else{
        setLookerEnabled(false);
    }
}

QModbusDataUnit LDMDevice::readRequest() const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = 0;

    quint16 numberOfEntries = 17;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}

//Хотим записать регистр
//Чтобы запрос на 6 ффункцию не улетел вместе с 3-ей, сохраняем параметры запроса и выставляем флаг
//Отправится по таймеру
void LDMDevice::setReg(int addr, int count, const QVector <unsigned short>& data){
    addrToSend = addr;
    counToSend = count;
    dataToSend = data;
    flag6func = true;
}

void LDMDevice::ackReady(){
    auto reply = qobject_cast<QModbusReply *>(sender());//ответ

    int servAdd = reply->serverAddress();               //Устройства откуда ответ
    if (!reply){
        m_looker->setEnabled(false);
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        m_looker->setEnabled(true);
        modbusRegs.clear();
        const QModbusDataUnit unit = reply->result();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i)
            modbusRegs.append(unit.value(i));
        QString str = "ACK FROM " +  QString::number(m_server) + " DEV: ";
        for(unsigned short i:modbusRegs)
            str+=QString::number(i) + " ";
        str+="\n";
        emit modbusDataReceved(m_server, str);
    }
    else{
         emit errorOccured("Write error (" + QString::number(servAdd) + " DEV): "+ reply->errorString()+"\n");
         m_looker->setEnabled(false);
    }
    reply->deleteLater();
}
QModbusDataUnit LDMDevice::writeRequest(int addr, int count) const
{
    const auto table = static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::HoldingRegisters);

    int startAddress = addr;
    quint16 numberOfEntries = count;
    return QModbusDataUnit(table, startAddress, numberOfEntries);
}
void LDMDevice::handlerReconnectionTimer()
{
    onConnect();//Пробуем переподключиться
    reconnectionTimer->stop();
}
