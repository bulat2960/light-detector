#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSize screenSize = QApplication::primaryScreen()->size();

    MainWindow w(screenSize);
    w.showFullScreen();

    // 1 - Device state
    // 2 - Delete write timeout
    // 3 - Add calibration
    // 4 - Add labels mainWindow binding


    return a.exec();
}

