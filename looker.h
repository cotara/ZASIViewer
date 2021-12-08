#ifndef LOOKER_H
#define LOOKER_H

#include <QGroupBox>

class Looker : public QGroupBox
{
    Q_OBJECT
public:
    explicit Looker(QWidget *parent = nullptr,int num=0);
    void setData(const QVector<double>& data);
    void setName(int name);
    virtual void setModel(int );
    virtual void rePaint();
    virtual ~Looker();
    QVector<double>& data();
signals:
    void onSetReg(int addr, int count, const QVector <unsigned short>& data);
private:
     QVector <double> m_data;
     int m_devNum;

};

#endif // LOOKER_H
