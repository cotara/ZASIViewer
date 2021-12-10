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
    centerviewer.cpp \
    connectionpanel.cpp \
    console.cpp \
    excelwriter.cpp \
    ldmdevice.cpp \
    looker.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    statusbar.cpp \
    zasilooker.cpp

HEADERS += \
    centerviewer.h \
    connectionpanel.h \
    console.h \
    excelwriter.h \
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

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./QXlsx/source/  # current QXlsx source path is ./source/
include(./QXlsx/QXlsx.pri)

DISTFILES += \
    QXlsx/QXlsx.pri
