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
signals:
    void connectionStatus(int status, const QString &host);
    void errorOccured(const QString &msg);
    void modbusDataReceved(int numDev, const QVector<short>& data);
private:
        QModbusClient *m_ModbusClient=nullptr,*m_ModbusClient1=nullptr, *m_ModbusClient2=nullptr;
        QTimer *m_timer1 =nullptr,*m_timer2 =nullptr;
        QString m_host1,m_host2;
        int m_server1=1,m_server2=2;
        QModbusDataUnit readRequest() const;

        QVector<short> modbusRegs1,modbusRegs2;
private slots:
        void onModbusStateChanged(int numDev, int state);
        void handlerTimer1();
        void handlerTimer2();
        void onReadReady1();
        void onReadReady2();
};

#endif // MODBUSCLIENT_H
