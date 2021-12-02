#ifndef DIAMETERLOOKER_H
#define DIAMETERLOOKER_H

#include "looker.h"
#include "centerviewer.h"

namespace Ui {
class diameterLooker;
}

class diameterLooker : public Looker
{
    Q_OBJECT

public:
    explicit diameterLooker(QWidget *parent = nullptr, int diam = 5, int num=0);
    virtual ~diameterLooker();

    virtual void setModel(int diam);
    virtual void rePaint();
private:
    Ui::diameterLooker *ui;
    int m_diam;//Диаметр прибора
    centerViewer *m_centerViewer; 
    void setError(int error);

};

#endif // DIAMETERLOOKER_H
