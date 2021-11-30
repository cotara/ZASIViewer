#ifndef CONNECTIONPANEL_H
#define CONNECTIONPANEL_H

#include <QGroupBox>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QRegExpValidator>

namespace Ui {
class ConnectionPanel;
}

class ConnectionPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit ConnectionPanel(QWidget *parent = nullptr);
    ~ConnectionPanel();
    QString getIpAdd(int number);
    int getPort(int number);
    int getServer(int number);
    int getDiam(int number);
    void setInterface(bool type){m_interface = type;interfaceSwitch(m_interface);}
    void setDoubleMode(bool type){m_doubleMode = type;oneTwoChange(m_doubleMode);}
public slots:
    void connectionChanged(int, int,const QString);

private slots:
    void interfaceSwitch(bool checked);
    void on_portsBox_currentIndexChanged(int index);
    void on_updAvblPortsButt_clicked();
    void on_connectButton_clicked();
    void oneTwoChange(int arg1);

private:
    Ui::ConnectionPanel *ui;
    void elementsDisable(bool state);
    bool m_interface=false,m_doubleMode = false;
signals:
    void connectionPushed(int numDev, QString ipadd,int port, int server);
    void connectionTypeChanged(bool type);
    void doubleModeChanged(bool mode);
    void server1changed(int serverAdd);
    void server2changed(int serverAdd);
    void diameter1changed(int diam);
    void diameter2changed(int diam);
    void clearConsole();
    void doubleButtonBlock(bool state);

};

#endif // CONNECTIONPANEL_H
