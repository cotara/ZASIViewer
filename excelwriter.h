#ifndef EXCELWRITER_H
#define EXCELWRITER_H

#include <QObject>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"


enum EntryType{
    ustChanged,
    ustSoftwared,//Изменена из программы
    ustInstarting,
    highChanged,
    highInstarting,
    countBangsChanged,
    countBangsInstarting,
    dropDeffectCount,
    dropDeffectCountFromHardware,//Сброшего из программы
    startMessage
};

class ExcelWriter : public QObject
{
    Q_OBJECT
public:
    explicit ExcelWriter(QObject *parent = nullptr);
    void writeToFile(int type, short value);
    void writeStartMessage(const QString& mes);
    void close();
    void setFileName(const QString & name);
private:
    int row=1,col=1,bangsCount=0;
    QString fileName;
    QXlsx::Document *m_xlsxFile;

};

#endif // EXCELWRITER_H
