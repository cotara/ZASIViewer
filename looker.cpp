#include "looker.h"

Looker::Looker(QWidget *parent, int num) : QGroupBox(parent), m_devNum(num){

    setTitle("Прибор №" + QString::number(m_devNum));

}
void Looker::setData(const QVector<double> &data)
{
    m_data = data;

}
void Looker::setName(int name){
    m_devNum = name;
    setTitle("Прибор №" + QString::number(name));
}



QVector<double> &Looker::data(){
    return m_data;
}
