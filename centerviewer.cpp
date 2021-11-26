#include "centerviewer.h"

centerViewer::centerViewer(QWidget *parent): QWidget(parent) {
    setObjectName("centerviewer");
    layoutV = new QVBoxLayout;
    setLayout(layoutV);

}

void centerViewer::setCoord(double x, double y)
{
    xPos=x;
    yPos=y;
    repaint();
}

void centerViewer::setRad(double x,double y)
{
    xRad=x;
    yRad=y;
}


void centerViewer::paintEvent(QPaintEvent *e) {

  Q_UNUSED(e);
  widgetCenter.setX(this->size().width()/2);
  widgetCenter.setY(this->size().height()/2);
  QPainter qp(this);
  addCircle(&qp);
  paintPosition(&qp,xRad,yRad);
}
void centerViewer::addCircle(QPainter *qp)
{

    qp->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));

    qp->drawEllipse(widgetCenter, widgetCenter.y(), widgetCenter.y());
    //Рисуем перекрестие
    QPoint d1(widgetCenter.x(),widgetCenter.y()+0.1*widgetCenter.y()),
           d2(widgetCenter.x(),widgetCenter.y()-0.1*widgetCenter.y()),
           d3(widgetCenter.x()-0.1*widgetCenter.y(),widgetCenter.y()),
           d4(widgetCenter.x()+0.1*widgetCenter.y(),widgetCenter.y());

    qp->drawLine(d1,d2);
    qp->drawLine(d3,d4);
}
void centerViewer::paintPosition(QPainter *qp,double xRad, double yRad)
{
    qp->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    QBrush brush(Qt::red,Qt::SolidPattern);
    qp->setBrush(brush);
    if(xRad > 0 && yRad>0)
        qp->drawEllipse(static_cast<QPointF>(widgetCenter), 5*xRad, 5*yRad);
}
