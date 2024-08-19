#include "looker.h"

Looker::Looker(QWidget *parent, int num) : QGroupBox(parent), m_devNum(num){
    setTitle("Прибор №" + QString::number(m_devNum));
}

void Looker::setData(const QVector<short> &data){
    m_data = data;
    rePaint();
}
void Looker::setName(int name){
    m_devNum = name;
    setTitle("Прибор №" + QString::number(name));
}

QVector<short> &Looker::data(){
    return m_data;
}
