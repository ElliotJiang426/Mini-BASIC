#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <ctime>

int main(int argc, char *argv[])
{
    srand(time(nullptr));
    QApplication a(argc, argv);
    mainwindow w;
    w.show();

    return a.exec();
}
