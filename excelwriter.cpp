#include "excelwriter.h"

ExcelWriter::ExcelWriter(QObject *parent) : QObject(parent){
    m_xlsxFile = new QXlsx::Document(this);
    m_xlsxFile->write(row,col++,"Протокол сессии работы аппарата высоковольтного испытательного ЗАСИ - 15М");
    m_xlsxFile->write(row,col++,QDateTime::currentDateTime());
    col=1;
    row+=2;
    m_xlsxFile->autosizeColumnWidth();
}

void ExcelWriter::writeToFile(int type, short value){
    switch (type) {
    case ustChanged:
        m_xlsxFile->write(row,col++,"Изменилась уставка...");
        m_xlsxFile->write(row,col++,value/100.0);
        break;
    case ustInstarting:
        m_xlsxFile->write(row,col++,"Уставка в начале сессии...");
        m_xlsxFile->write(row,col++,value);
        break;
    case highChanged:
        m_xlsxFile->write(row,col++,"Изменено значение высокго напряжения...");
        m_xlsxFile->write(row,col++,value);
        break;
    case highInstarting:
        m_xlsxFile->write(row,col++,"Высокое в начале сесии...");
        m_xlsxFile->write(row,col++,value);
        break;
    case countBangsChanged:
        m_xlsxFile->write(row,col++,"Произошел пробой...");
        m_xlsxFile->write(row,col++,value);
        break;
    case countBangsInstarting:
        m_xlsxFile->write(row,col++,"Количество дефектов в начале сессии...");
        m_xlsxFile->write(row,col++,value);
        row++;
        bangsCountInStarting=value;
        bangsCount=value;
        break;
    case dropDeffectCount:
        m_xlsxFile->write(row,col++,"Произошел пробой...");
        m_xlsxFile->write(row,col++,value);
        bangsCount=value;
        break;
    }
    col=1;
    row++;
}

void ExcelWriter::close(const QString &name){
    row++;
    if(bangsCount==0)
        bangsCount=
    m_xlsxFile->write(row,col++,"Количество выявленных дефектов...");
    if(bangsCount-bangsCountInStarting < 0)
        m_xlsxFile->write(row,col++,0);
    else
        m_xlsxFile->write(row,col++,bangsCount-bangsCountInStarting);
    col=1;
    row++;
    m_xlsxFile->write(row,col++,"Общее количество дефектов...");
    m_xlsxFile->write(row,col++,bangsCount);

    m_xlsxFile->saveAs(name);
}
