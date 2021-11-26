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
    void onConnect(int numDev, bool type, QString ipadd,int port, int server);
    void onConnectTypeChanged(bool type);
    void setServer1(int serverAdd);
    void setServer2(int serverAdd);
    void setDoubleMode(bool doubleMode);
signals:
    void connectionStatus(int status, const QString &host);
    void errorOccured(int numDev, const QString &msg);
    void modbusDataReceved(int numDev, const QVector<short>& data);
    void modbusRequestSent(int numDev, const QByteArray &req);
private:
        QModbusClient *m_ModbusClient=nullptr,*m_ModbusClient1=nullptr, *m_ModbusClient2=nullptr;
        QTimer *m_timer =nullptr;
        QString m_host1,m_host2;
        int m_server1=0,m_server2=0;
        bool m_type=false,m_doubleMode=false;
        QModbusDataUnit readRequest() const;
        QVector<short> modbusRegs;
private slots:
        void onModbusStateChanged(int numDev, int state);
        void handlerTimer();
        void onReadReady();
};

#endif // MODBUSCLIENT_H
