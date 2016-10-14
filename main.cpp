#include <QApplication>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    srand(a.applicationPid());

    Widget w;
    w.show();
    
    return a.exec();
}
