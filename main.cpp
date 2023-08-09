#include <QtWidgets/QApplication>

#include "gui.h"
#include "ca.h"
#include "CaThread.h"

#define CAMERA_ID 0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui w;

    ca* cap;
    CaThread* ct;

    ct = new CaThread(w.ui.rawlabel, w.ui.binlabel);
    ct->start();

    QObject::connect(w.ui.thresholdslider, &QSlider::valueChanged, [&](int value) {
        cap->setThreshG(value);
        w.ui.threshold->setValue(value);
    });

    QObject::connect(w.ui.threshold, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        cap->setThreshG(value);
        w.ui.thresholdslider->setValue(value);
    });

    QObject::connect(w.ui.cameraopen, &QPushButton::clicked, [&]() {
        if (w.cameraStatus) {
            w.setCameraStatus(false);
            ct->close();
        }
        else {
            cap = new ca(w.ui.cameraid->value());
            cap->setCubeInfo(w.ui.cubecount->value(), w.ui.cubesize->value(), w.ui.cubemargin->value());

            w.setCameraStatus(true);

            ct->open(cap);
        }
    });

    w.show();
    w.setFixedWidth(w.width());
    int rnt = a.exec();

    ct->requestInterruption();
    ct->wait();
    if (ct->getCameraFlag()) {
        delete ct;
        delete cap;
    }
    else {
        delete ct;
    }
    
    return rnt;
}
