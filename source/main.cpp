#include "mainDialog.h"

#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDialog w;
    w.show();
    return a.exec();
}
