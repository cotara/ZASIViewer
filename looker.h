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
private:
    Ui::Looker *ui;
    diameterLooker *diamLooker1,*diamLooker2;
    int numDev1=0,numDev2=0;//Каждому лукеру ставится в соответствие номер сервера, чтобы отображать данные

};

#endif // LOOKER_H
