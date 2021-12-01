#ifndef LDMDEVICE_H
#define LDMDEVICE_H

#include <QWidget>
#include <QVariant>
#include <QSerialPort>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QMessageBox>
#include <QTimer>
#include "diameterlooker.h"
#include "looker.h"


enum ConnectionType {
    Serial,
    Tcp
};
enum DeviceType {
    LDM,
    ZASI
};

class LDMDevice : public QWidget
{
    Q_OBJECT
public:
    explicit LDMDevice(QWidget *parent= nullptr, const QString & ipAdd="", int port=0, int server=0, bool type=false,int model=50,int deviceType=LDM);
    LDMDevice(QWidget *parent= nullptr,QModbusClient* modbusClient= nullptr,int server=0, bool type=false,int model=50,int deviceType=LDM);
    ~LDMDevice();
    QModbusClient* getModbusClient();
public slots:
    void connectionTypeChanged(bool type);
    void onConnect();
    void onDisconnect();
    void setServer(int serverAdd);
    void setIpAdd_comp(const QString& ipAdd_comp);
    void setPort_boud(int port_boud);
    void setModel(int model);
private:
    QVBoxLayout *VLayout;
    QModbusClient *m_ModbusClient;
    Looker *m_looker;
    QTimer *m_timer =nullptr;
    QString m_ipAdd_comp;
    int m_port_boud;
    int m_server;
    bool m_type=Serial;
    int m_model;
    QVector<unsigned short> modbusRegs;


private slots:
    void onModbusStateChanged(int state);
    void handlerTimer();
    void onReadReady();
    QModbusDataUnit readRequest() const;

signals:
    void connectionStatus(int server, int status, const QString &host);
    void errorOccured(const QString &msg);
    void modbusDataReceved(int server, const QVector<unsigned short>& data);
    void modbusRequestSent(int server, const QByteArray &req);

};

#endif // LDMDEVICE_H


