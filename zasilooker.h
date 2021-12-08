#ifndef ZASILOOKER_H
#define ZASILOOKER_H

#include <QWidget>
#include "looker.h"
#include <QDateTime>

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"


namespace Ui {
class ZasiLooker;
}

class ZasiLooker : public Looker
{
    Q_OBJECT

public:
    explicit ZasiLooker(QWidget *parent = nullptr, int diam=5, int num=0);
    ~ZasiLooker();
    virtual void rePaint();

private slots:

    void on_setUstButton_clicked();
    void on_DropDefectCountButton_clicked();
    void on_onHighButton_clicked(bool checked);
    void writeToExcel();
private:
    Ui::ZasiLooker *ui;
    double m_model,m_vers,m_ustMin,m_ustMax,m_ustCur,m_highCur,m_voltageCur,m_countBang;
    bool m_isHigh;
    const QString lightgreen = "QLabel { background-color : lightgreen; }";
    const QString yellow = "QLabel { background-color : yellow; }";
    const QString red = "QLabel { background-color : red; color : white }";
    QString fname;
    int m_current_xlsx_line;


};

#endif // ZASILOOKER_H

//Адрес регистра	Хранимое значение	                         Режим доступа
//0x0000              модель прибора                                  Чтение
//0x0001              версия прошивки                                 Чтение
//0x0002              минимальная уставка                             Чтение
//0x0003              максимальная уставка                            Чтение
//0x0004              сохраненное значение регулятора в точке 1       Чтение/ запись
//0x0005              сохраненное значение регулятора в точке 2       Чтение/ запись
//0x0006              сохраненное значение напряжения в точке 1       Чтение/ запись
//0x0007              сохраненное значение напряжения в точке 2       Чтение/ запись
//0x0008              режим работы прибора                            Чтение/ запись
//0x0009              выход регулятора при калибровке                 Чтение/ запись
//0x000A	        высокое вкл/выкл                	        Чтение/ запись
//0x000B  	        Текущее значение реальное высокое               Чтение
//0x000C	        Текущее значение уставки по напряжению	        Чтение/ запись
//0x000D	        Текущее значение напряжения                     Чтение
//0x000E	        Отображаемое значение                           Чтение
//0x000F	        Текущее значение емкости                        Чтение
//0x0010	        Текущее значение количества пробоев             Чтение/ запись
