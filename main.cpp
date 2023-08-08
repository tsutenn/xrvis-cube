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
    CaThread ct(&cap, w.ui.rawlabel, w.ui.binlabel);
    ct.start();

    QObject::connect(w.ui.thresholdslider, &QSlider::valueChanged, [&](int value) {
        cap.setThreshG(value);
        w.ui.threshold->setValue(value);
    });

    QObject::connect(w.ui.threshold, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        cap.setThreshG(value);
        w.ui.thresholdslider->setValue(value);
    });
    
    int rnt = a.exec();
    ct.requestInterruption();
    ct.wait();

    return rnt;
}
