#include "StartDialog.h"
#include <QApplication>
#include <iostream>
#include <cmath>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StartDialog settingsDialog;
    settingsDialog.show();

    return a.exec();
}
