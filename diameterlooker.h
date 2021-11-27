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
    void setData(const QVector<double>& data);

private:
    Ui::diameterLooker *ui;
    QVector <double> m_data;
    centerViewer *m_centerViewer;

};

#endif // DIAMETERLOOKER_H
