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
    void setDoubleMode(bool mode){m_doubleMode = mode;oneTwoChange(m_doubleMode);}
public slots:
    void connectionChanged(int, int,const QString);
    void interfaceSwitch(bool type);
    void setStatusLabel(int numDev,bool state);
private slots:
    void on_portsBox_currentIndexChanged(int index);
    void on_updAvblPortsButt_clicked();
    void on_connectButton_clicked();
    void oneTwoChange(int arg1);

private:
    Ui::ConnectionPanel *ui;
    void elementsDisable(bool state);
    bool m_interface=false,m_doubleMode = false;


    const QString lightgreen = "QLabel { background-color : lightgreen; }";
    const QString yellow = "QLabel { background-color : yellow; }";
    const QString red = "QLabel { background-color : red; color : white }";

signals:
    void connectionPushed(bool action);
    void ipAdd_compChanged(int numDev,const QString &ipAdd_comp);
    void port_boudChanged(int numDev,int port_boud);
    void serverChanged(int numDev, int serverAdd);
    void modelChanged(int numDev,int model);
    void clearConsole();
    void doubleButtonBlock(bool state);

};

#endif // CONNECTIONPANEL_H
