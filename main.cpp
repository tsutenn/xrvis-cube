#include <QtWidgets/QApplication>

#include "gui.h"
#include "ca.h"
#include "CaThread.h"
#include "MyLog.h"

#define CAMERA_ID 0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui w;

    ca* cap;
    CaThread* ct;

    ct = new CaThread(w.ui.rawlabel, w.ui.binlabel, w.ui.edge);
    ct->start();

    QObject::connect(w.ui.thresholdslider, &QSlider::valueChanged, [&](int value) {
        if (ct->getCameraFlag()) {
            cap->setThreshG(value);
            w.setThreshold(value);
        }
        else {
            w.ui.threshold->setValue(value);
        }
    });

    QObject::connect(w.ui.threshold, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        if (ct->getCameraFlag()) {
            cap->setThreshG(value);
            w.setThreshold(value);
        }
        else {
            w.ui.thresholdslider->setValue(value);
        }
    });

    QObject::connect(w.ui.cameraopen, &QPushButton::clicked, [&]() {
        if (w.cameraStatus) {
            w.setCameraStatus(false);

            w.Log("CAMERA CLOSED");
            ct->close();
        }
        else {
            w.setCameraStatus(true);

            cap = new ca(w.camera_id);
            cap->setCubeInfo(w.marker_size, w.marker_length, w.marker_margin, w.cube_count);
            cap->setThreshG(w.threshold);

            w.Log("CAMERA OPENED (camera_id=" + QString::number(w.camera_id) +
                ", marker_size=" + QString::number(w.marker_size) +
                ", marker_length=" + QString::number(w.marker_length) +
                ", marker_margin=" + QString::number(w.marker_margin) +
                ", cube_count=" + QString::number(w.cube_count) +
                ", threshold=" + QString::number(w.threshold) +
                ")");
            ct->open(cap);
        }
    });

    w.move(0, 0);
    w.show();
    w.setFixedSize(w.size());

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
