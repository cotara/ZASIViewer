#ifndef LDMDEVICE_H
#define LDMDEVICE_H

#include <QWidget>
#include <QVariant>
#include <QVBoxLayout>
#include <QSerialPort>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QMessageBox>
#include <QTimer>
#include "zasilooker.h"
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
    void createNewClien(QModbusClient *client,bool type);
    void onConnect();
    void onDisconnect();
    void setServer(int serverAdd){
        m_server = serverAdd;
        m_looker->setName(serverAdd);//меняем заголовок в лукере
    }
    void setIpAdd_comp(const QString& ipAdd_comp){m_ipAdd_comp = ipAdd_comp; }
    void setPort_boud(int port_boud){ m_port_boud = port_boud;}
    void setModel(int model){ m_model = model; m_looker->setModel(model); }
    int getServer(){return m_server;};
    void setLookerEnabled(bool state){m_looker->setEnabled(state);};
    void setLogProperty(bool state){m_looker->loggingProperty = state;}
    void setReconnection(bool flag);
private:
    QVBoxLayout *VLayout;
    QModbusClient *m_ModbusClient=nullptr;
    Looker *m_looker=nullptr;
    QTimer *m_timer =nullptr;
    QString m_ipAdd_comp;
    int m_port_boud;
    int m_server;
    bool m_type=Serial;
    int m_model;
    QVector<unsigned short> modbusRegs;
    bool isMaster = false;//Переменная, указывающая на то, что это главное устройство из всех, созданных.
    //Это важно для Serial, т.к. удалять можно только m_ModbusClient, т.к. он общий

    int addrToSend = 0, counToSend=0;
    QVector <unsigned short> dataToSend;
    bool flag6func=false;
    int errorsCounter=0;
    bool reconnectinFlag=true;

private slots:
    void onModbusStateChanged(int state);
    void handlerTimer();
    void onReadReady();
    void ackReady();
    void setReg(int addr, int count,const QVector <unsigned short>& data);
    QModbusDataUnit readRequest() const;
    QModbusDataUnit writeRequest(int addr, int count) const;
    void modbusDataProcessing();
signals:
    void connectionStatus(int server, int status, const QString &host);
    void errorOccured(const QString &msg);
    void modbusDataReceved(int server, const QString &str);
    void modbusRequestSent(int server, const QString &str);
    void modelChanged(int model);

};

#endif // LDMDEVICE_H


