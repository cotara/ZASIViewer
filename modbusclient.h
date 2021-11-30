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
    void onConnect(QString ipadd,int port, int server);
    void onConnectTypeChanged(bool type);
    void setServer(int serverAdd);
signals:
    void connectionStatus(int server, int status, const QString &host);
    void errorOccured(const QString &msg);
    void modbusDataReceved(int numDev, const QVector<unsigned short>& data);
    void modbusRequestSent(int numDev, const QByteArray &req);
private:
        QModbusClient *m_ModbusClient;
        QTimer *m_timer =nullptr;
        QString m_host;
        int m_server;
        bool m_type=false;
        QModbusDataUnit readRequest() const;
        QVector<unsigned short> modbusRegs;
private slots:
        void onModbusStateChanged(int state);
        void handlerTimer();
        void onReadReady();
};

#endif // MODBUSCLIENT_H
