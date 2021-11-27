#include "diameterlooker.h"
#include "ui_diameterlooker.h"

diameterLooker::diameterLooker(QWidget *parent, int num, int diam) :  QGroupBox(parent), ui(new Ui::diameterLooker)
{
    ui->setupUi(this);
    setTitle("Прибор №" + QString::number(num));
    m_diam = diam;
    m_centerViewer = new centerViewer(this,m_diam);
    ui->VLayout->addWidget(m_centerViewer);

}

diameterLooker::~diameterLooker()
{
    delete ui;
    delete m_centerViewer;
}

void diameterLooker::setData(const QVector<double> &data)
{
    m_data = data;
    ui->lcdNumber->display(m_data.at(0));
    ui->lcdNumber_2->display(m_data.at(1));
    ui->lcdNumber_3->display(m_data.at(2));
    m_centerViewer->setCoord(m_data.at(3),m_data.at(4));
    m_centerViewer->setRad(m_data.at(1),m_data.at(2));
}

void diameterLooker::setDiam(int diam)
{
    m_diam = diam;
    m_centerViewer->setScale(m_diam);
}
