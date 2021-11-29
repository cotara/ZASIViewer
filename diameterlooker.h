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
    explicit diameterLooker(QWidget *parent = nullptr,int num=0, int diam = 5);
    ~diameterLooker();
    void setData(const QVector<double>& data);
    void setDiam(int diam);
private:
    Ui::diameterLooker *ui;
    QVector <double> m_data;
    centerViewer *m_centerViewer;
    int m_diam;//Диаметр прибора
    void setError(int error);

};

#endif // DIAMETERLOOKER_H
