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
    void setData(double number,int dataType, int numDev);

private:
    Ui::Looker *ui;
    diameterLooker *diamLooker1,*diamLooker2;
};

#endif // LOOKER_H
