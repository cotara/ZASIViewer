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
    int getDevNum(int number);

public slots:
    void connectionChanged(int,const QString);

private slots:
    void on_m_switch_clicked(bool checked);
    void on_portsBox_currentIndexChanged(int index);
    void on_updAvblPortsButt_clicked();
    void on_connectButton_clicked();

    void on_oneTwoCheck_stateChanged(int arg1);

private:
    Ui::ConnectionPanel *ui;
    void elementsDisable(bool state);

signals:
    void connectionPushed(int numDev, bool type, QString ipadd,int port, int server);
    void connectionTypeChanged(bool type);
    void doubleModeChanged(bool mode);
    void server1changed(int serverAdd);
    void server2changed(int serverAdd);
    void clearConsole();

};

#endif // CONNECTIONPANEL_H
