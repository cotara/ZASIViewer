#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) , ui(new Ui::MainWindow){
    ui->setupUi(this);

    HLayout = new QHBoxLayout();
    VLayout = new QVBoxLayout();
    centralWidget()->setLayout(HLayout);

    m_connectionPanel = new ConnectionPanel(this);
    m_console = new Console(this);
    m_console->setMaximumHeight(300);
    m_console->setMinimumHeight(200);
    m_looker = new Looker(this);
    m_looker->setMinimumHeight(500);
    m_statusBar = new StatusBar(ui->statusbar);

    HLayout->addLayout(VLayout);
    HLayout->addWidget(m_connectionPanel);

    VLayout->addWidget(m_looker);
    VLayout->addWidget(m_console);


    m_connectionPanel->setMaximumWidth(250);

    m_modbusClient = new ModBusClient(this);

    connect(m_connectionPanel,&ConnectionPanel::connectionPushed,m_modbusClient,&ModBusClient::onConnect);//Когда нажата кнопка пробуем подл/откл
    connect(m_modbusClient,&ModBusClient::connectionStatus, m_connectionPanel,&ConnectionPanel::connectionChanged);//Когда подл/откл меняем кнопку
    connect(m_modbusClient,&ModBusClient::connectionStatus, this,&MainWindow::connectionChanged);               //Когда подл/откл выводим в бар
    connect(m_modbusClient,&ModBusClient::errorOccured, this,[=](const QString &msg){m_statusBar->setMessageBar(msg);});               //Сообщение об ошибке

    connect(m_modbusClient,&ModBusClient::modbusDataReceved,this,&MainWindow::tcpTextProcessing);                   //Пришли данные по modbus
    connect(m_connectionPanel,&ConnectionPanel::connectionTypeChanged,m_modbusClient,&ModBusClient::onConnectTypeChanged);//Изменился тип подключения


}

MainWindow::~MainWindow()
{

    delete ui;
    delete m_console;
    delete m_console;
    delete m_looker;
    delete m_statusBar;
    delete m_modbusClient;

}

void MainWindow::connectionChanged(int status,const QString &host){

    switch(status){
        case 0:   //Отключено
            m_statusBar->setMessageBar("Отключено от " + host);
            break;
        case 1:    //Подключение
           m_statusBar->setMessageBar("Подключение к " + host);
        break;
        case 2:     //Подключено
            m_statusBar->setMessageBar("Подключено к " + host);
            break;
        case 3:       //Отключение
            m_statusBar->setMessageBar("Отключение от " + host);
        break;
    }

}

void MainWindow::tcpConnectionFailed(){
    QMessageBox::critical(this,"Ошибка!","Подключение не удалось!");
}

//Обработчки данных, пришедших от устройства
void MainWindow::tcpTextProcessing(int numDev, const QVector<short>& data){
    QByteArray arr,temp;
    arr.append("DEV ");
    arr.append(QString::number(numDev).toUtf8());
    arr.append(": ");

    for(short i:data){
        temp.setNum(i);
        arr.append(temp);
        arr.append(" ");
    }
    arr.append("\n");
    m_console->putData(arr);

    m_looker->setData(data.at(1)/1000.0,0,numDev);
    m_looker->setData(data.at(3)/1000.0,1,numDev);
    m_looker->setData(data.at(5)/1000.0,2,numDev);

}

