#include "StaubilWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StaubilWidget w;
    w.show();
    return a.exec();
}
