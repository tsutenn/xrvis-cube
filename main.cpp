#include <QtWidgets/QApplication>

#include "gui.h"
#include "ca.h"
#include "CaThread.h"

#define CAMERA_ID 0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui w;
    w.show();

    ca cap(CAMERA_ID);
    CaThread ct(&cap, w.ui.raw_label, w.ui.bin_label);
    ct.start();
    
    int rnt = a.exec();
    ct.requestInterruption();
    ct.wait();

    return rnt;
}
