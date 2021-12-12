#ifndef LOOKER_H
#define LOOKER_H

#include <QGroupBox>

class Looker : public QGroupBox
{
    Q_OBJECT
public:
    explicit Looker(QWidget *parent = nullptr,int num=0);
    virtual ~Looker()  {   }
    void setData(const QVector<short>& data);
    void setName(int name);
    QVector<short>& data();

    virtual void setModel(int ){ }
    virtual void rePaint()     { }
    virtual void onConnect(bool state)  { }

    bool loggingProperty=false;//Включено ли логирование

signals:
    void onSetReg(int addr, int count, const QVector <unsigned short>& data);
    void modelChanged(int model);

private:
     QVector <short> m_data;
     int m_devNum;
};

#endif // LOOKER_H
