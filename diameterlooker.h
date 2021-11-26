#ifndef DIAMETERLOOKER_H
#define DIAMETERLOOKER_H

#include <QGroupBox>
#include "centerviewer.h"

namespace Ui {
class diameterLooker;
}

class diameterLooker : public QGroupBox
{
    Q_OBJECT

public:
    explicit diameterLooker(QWidget *parent = nullptr, int num = 0);
    ~diameterLooker();
    void setDataD(double data);
    void setDataDX(double data);
    void setDataDY(double data);

private:
    Ui::diameterLooker *ui;
    double d=0,dx=0,dy=0;
};

#endif // DIAMETERLOOKER_H
