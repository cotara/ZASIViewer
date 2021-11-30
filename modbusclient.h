#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>

#include <QModbusTcpClient>
#include <QMessageBox>
#include <QTimer>
class ModBusClient : public QObject
{
    Q_OBJECT
public:
    explicit ModBusClient(QObject *parent = nullptr);
    ~ModBusClient();
public slots:
    void onConnect(int numDev, QString ipadd,int port, int server);
    void onConnectTypeChanged(bool type);
    void setServer(int numDev, int serverAdd);
    void setDoubleMode(bool doubleMode);
signals:
    void connectionStatus(int server, int status, const QString &host);
    void errorOccured(int server, const QString &msg);
    void modbusDataReceved(int numDev, const QVector<unsigned short>& data);
    void modbusRequestSent(int numDev, const QByteArray &req);
private:
        //QModbusClient *m_ModbusClient=nullptr,*m_ModbusClient1=nullptr, *m_ModbusClient2=nullptr;
        QVector <QModbusClient*> m_ModbusClients;
        QTimer *m_timer =nullptr;
        QVector<QString> m_hosts;
        QVector<int> m_servers;
        bool m_type=false,m_doubleMode=false;
        QModbusDataUnit readRequest() const;
        QVector<unsigned short> modbusRegs;
private slots:
        void onModbusStateChanged(int numDev,int server, int state);
        void handlerTimer();
        void onReadReady();
};

#endif // MODBUSCLIENT_H
