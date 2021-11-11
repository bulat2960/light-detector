#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSize screenSize = QApplication::primaryScreen()->size();

    MainWindow w(screenSize);
    w.setMinimumSize(screenSize * 0.75);
    w.showMaximized();

    // TODO: Add validators

    return a.exec();
}

