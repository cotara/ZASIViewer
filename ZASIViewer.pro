QT       += core gui serialport network serialbus printsupport

VERSION = 1.0
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

EXTRA_QMAKE_TARGET_PRODUCT =  LDM Viewer
QMAKE_TARGET_DESCRIPTION = Клиент для измерителей диаметров LDM
QMAKE_TARGET_COPYRIGHT = NurRuslan

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/snippets/doc_src_qtxlsx.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractooxmlfile.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractsheet.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcell.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellformula.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellrange.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellreference.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchart.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchartsheet.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcolor.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxconditionalformatting.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcontenttypes.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdatavalidation.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocpropsapp.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocpropscore.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocument.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdrawing.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdrawinganchor.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxformat.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxmediafile.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxnumformatparser.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxrelationships.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxrichstring.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxsharedstrings.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxsimpleooxmlfile.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxstyles.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxtheme.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxutility.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworkbook.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworksheet.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxzipreader.cpp \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxzipwriter.cpp \
    centerviewer.cpp \
    connectionpanel.cpp \
    console.cpp \
    ldmdevice.cpp \
    looker.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    statusbar.cpp \
    zasilooker.cpp

HEADERS += \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractooxmlfile.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractooxmlfile_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractsheet.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxabstractsheet_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcell.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcell_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellformula.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellformula_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellrange.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcellreference.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchart.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchart_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchartsheet.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxchartsheet_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcolor_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxconditionalformatting.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxconditionalformatting_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxcontenttypes_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdatavalidation.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdatavalidation_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocpropsapp_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocpropscore_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocument.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdocument_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdrawing_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxdrawinganchor_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxformat.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxformat_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxglobal.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxmediafile_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxnumformatparser_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxrelationships_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxrichstring.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxrichstring_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxsharedstrings_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxsimpleooxmlfile_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxstyles_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxtheme_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxutility_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworkbook.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworkbook_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworksheet.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxworksheet_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxzipreader_p.h \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsxzipwriter_p.h \
    centerviewer.h \
    connectionpanel.h \
    console.h \
    ldmdevice.h \
    looker.h \
    mainwindow.h \
    qcustomplot.h \
    statusbar.h \
    zasilooker.h

FORMS += \
    connectionpanel.ui \
    mainwindow.ui \
    zasilooker.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
RC_ICONS += icon.ico

SUBDIRS += \
    QtXlsxWriter-master/QtXlsxWriter-master/src/src.pro \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/snippets/doc_src_qtxlsx.pro \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/xlsx.pro

DISTFILES += \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/qtxlsx.qdocconf \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/src/examples.qdoc \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/src/qtxlsx-index.qdoc \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/src/qtxlsx.qdoc \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/doc/src/usage.qdoc \
    QtXlsxWriter-master/QtXlsxWriter-master/src/xlsx/qtxlsx.pri


