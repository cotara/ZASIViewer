#ifndef LOOKER_H
#define LOOKER_H

#include <QWidget>
#include "diameterlooker.h"

namespace Ui {
class Looker;
}

class Looker : public QWidget
{
    Q_OBJECT

public:
    explicit Looker(QWidget *parent = nullptr);
    ~Looker();
    void setData(const QVector <double>& data, int numDev);
public slots:
    void setNumDev1(int num);
    void setNumDev2(int num);
    void setDiam1(int diam);
    void setDiam2(int diam);
    void enableSecondLooker(bool state);
private:
    Ui::Looker *ui;
    diameterLooker *diamLooker1,*diamLooker2;
    int numDev1=0,numDev2=0;//Каждому лукеру ставится в соответствие номер сервера, чтобы отображать данные
    int diam1 = 5,diam2 = 5;//Каждому лукеру становится в соответствие диаметр по модели прибора
};

#endif // LOOKER_H
