#include "diameterlooker.h"
#include "ui_diameterlooker.h"

diameterLooker::diameterLooker(QWidget *parent, int num) :  QGroupBox(parent), ui(new Ui::diameterLooker)
{
    ui->setupUi(this);
    setTitle("Прибор №" + QString::number(num));
    m_centerViewer = new centerViewer(this);
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
    ui->lcdNumber->display(0);
    ui->lcdNumber_2->display(1);
    ui->lcdNumber_3->display(2);
    m_centerViewer->setCoord(m_data.at(3),m_data.at(4));
    m_centerViewer->setRad(m_data.at(1),m_data.at(2));
}
