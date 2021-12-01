#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionpanel.h"
#include "console.h"
#include <QHBoxLayout>
#include "statusbar.h"
#include "ldmdevice.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QHBoxLayout *HLayout,*devicesLayout;
    QVBoxLayout *VLayout;

    StatusBar *m_statusBar;
    QVector <LDMDevice*> devices;
    ConnectionPanel *m_connectionPanel;
    Console *m_console;


private slots:
    void connectionChanged(int numDev, int status,const QString &host);
    void connectionFailed(const QString &msg);
    void connectionPushed(bool action);
    void modbusReqPrint(int numDev,const  QByteArray &reqeq);
    void modbusDataProcessing(int numDev, const QVector<unsigned short>& data);
    void clearConsole();
    void on_actionconsoleOn_toggled(bool arg1);
    void on_actionsettingsOn_toggled(bool arg1);
    void on_actiontcp_com_toggled(bool arg1);
    void on_actiondoubleMode_toggled(bool arg1);

};
#endif // MAINWINDOW_H
