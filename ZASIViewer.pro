QT       += core gui serialport network serialbus printsupport

VERSION = 1.0
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

EXTRA_QMAKE_TARGET_PRODUCT =  ZASI Viewer
QMAKE_TARGET_DESCRIPTION = Клиент для ZASI
QMAKE_TARGET_COPYRIGHT = NurRuslan

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH = ../../Redwill/

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


RESOURCES += \
    res.qrc
RC_ICONS += icon.ico

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=../../Redwill//QXlsx/         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=../../Redwill//QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=../../Redwill//QXlsx/source/  # current QXlsx source path is ./source/
include(../../Redwill/QXlsx/QXlsx.pri)


include(../../Redwill/Redwill.pri)
