#include "excelwriter.h"

ExcelWriter::ExcelWriter(QObject *parent) : QObject(parent){
    m_xlsxFile = new QXlsx::Document(this);
}

ExcelWriter::~ExcelWriter(){
    delete m_xlsxFile;
}
//Стартовая запись
void ExcelWriter::writeStartMessage(const QString &mes){
    col++;
    m_xlsxFile->write(row,col++,"Протокол сессии работы аппарата высоковольтного испытательного ЗАСИ - " + mes);
    m_xlsxFile->write(row,col++,QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
    col=1;
    row+=2;
    m_xlsxFile->autosizeColumnWidth();
    m_xlsxFile->saveAs(fileName);
}

void ExcelWriter::writeToFile(int type, short value){

    m_xlsxFile->write(row,col++,QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
    switch (type) {
    case ustChanged:       
        m_xlsxFile->write(row,col++,"Изменена уставка по нарпяжению оператором на линии:");
        m_xlsxFile->write(row,col++,value/100.0);
        m_xlsxFile->write(row,col++,"кВ");
        break;
    case ustSoftwared:
        m_xlsxFile->write(row,col++,"Изменена уставка по нарпяжению:");
        m_xlsxFile->write(row,col++,value/100.0);
        m_xlsxFile->write(row,col++,"кВ");
        break;
    case ustInstarting:
        m_xlsxFile->write(row,col++,"Уставка по напряжению в начале сессии:");
        m_xlsxFile->write(row,col++,value/100.0);
        m_xlsxFile->write(row,col++,"кВ");
        break;
    case highChanged:
        if(value == 1)
             m_xlsxFile->write(row,col++,"Высокое напряжение включено");
        else if(value == 0)
             m_xlsxFile->write(row,col++,"Высокое напряжение выключено");
        break;
    case highInstarting:
        if(value == 1)
             m_xlsxFile->write(row,col++,"Высокое напряжение в начале сесии включено");
        else if(value == 0)
             m_xlsxFile->write(row,col++,"Высокое напряжение в начале сесии выключено");
        break;
    case countBangsChanged:
        m_xlsxFile->write(row,col++,"Произошел пробой, общее количество:");
        m_xlsxFile->write(row,col++,value);
        bangsCount++;
        break;
    case countBangsInstarting:
        m_xlsxFile->write(row,col++,"Количество дефектов в начале сессии:");
        m_xlsxFile->write(row,col++,value);
        row++; 
        break;
    case dropDeffectCount:
        m_xlsxFile->write(row,col++,"Сброшен счетчик дефектов, общее количество:");
        m_xlsxFile->write(row,col++,value);
        break;
    case dropDeffectCountFromHardware:
        m_xlsxFile->write(row,col++,"Сброшен счетчик дефектов оператором на линии, общее количество:");
        m_xlsxFile->write(row,col++,value);
        break;
    }
    col=1;
    row++;
    m_xlsxFile->autosizeColumnWidth();
    m_xlsxFile->saveAs(fileName);
}
//Завершающая запись
void ExcelWriter::close(){
    row++;
    m_xlsxFile->write(row,col++,"Количество выявленных дефектов");
    m_xlsxFile->write(row,col++,bangsCount);
    m_xlsxFile->autosizeColumnWidth();
    m_xlsxFile->saveAs(fileName);
}

void ExcelWriter::setFileName(const QString &name){
    fileName = name;
}
