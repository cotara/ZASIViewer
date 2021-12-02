#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    w.setWindowTitle("LDMViewer");
    w.setWindowIcon(QIcon(":/res/target.png"));
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/Nunito/Nunito-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/GranaPadano.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Light.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Resources/Fonts/SF/SFUIText-Regular.ttf");
    QFile file(":/qss/style.css");
    if(file.open(QFile::ReadOnly)){
          QByteArray style = file.readAll();
          a.setStyleSheet(style);
          file.close();
    }


    w.show();
    return a.exec();
}
