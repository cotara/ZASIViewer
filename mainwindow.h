#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionpanel.h"
#include "console.h"
#include <QHBoxLayout>
#include "looker.h"
#include "statusbar.h"
#include "modbusclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ConnectionPanel *m_connectionPanel;
    Console *m_console;
    Looker *m_looker;

    QHBoxLayout *HLayout;
    QVBoxLayout *VLayout;
private:
    Ui::MainWindow *ui;
    StatusBar *m_statusBar;
    ModBusClient *m_modbusClient;
private slots:
    void connectionChanged(int status,const QString &host);
    void tcpConnectionFailed();
    void tcpTextProcessing(int numDev, const QVector<short>& data);
};
#endif // MAINWINDOW_H
