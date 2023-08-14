#include <QtWidgets/QApplication>

#include "ca.h"

#include "gui.h"
#include "msg.h"
#include "CaThread.h"

#define CAMERA_ID 0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    msg mydata;
    gui w(&mydata);

    QObject::connect(&mydata, &msg::Log, &w, &gui::Log);
    QObject::connect(w.ui.cubesload, &QPushButton::clicked, [&] {
        if (mydata.loadMarkerList(w.ui.pathbox->text())) {
            mydata.path = w.ui.pathbox->text();
            mydata.Log("Successfully load cube info at " + mydata.path);

            //QString result("");
            //for (int i = 0; i < mydata.marker_size; i++) {
            //    for (int j = 0; j < mydata.marker_size; j++) {
            //        result += QString::number(mydata.cubes[0].Face(1).At(j, i)) + ",";
            //    }
            //    result += "\n";
            //}
            //mydata.Log(result);
        }
        else {
            mydata.Log("Load file failed");
        }
    });

    ca* cap;
    CaThread* ct;

    ct = new CaThread(&mydata, w.ui.rawlabel, w.ui.binlabel, w.ui.edge);
    ct->start();

    ct->setOutput(w.ui.outputlabel);

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
        if (mydata.camera_status) {
            w.setCameraStatus(false);
            ct->close();

            mydata.Log("CAMERA CLOSED");
        }
        else {
            w.setCameraStatus(true);

            cap = new ca(mydata.camera_id);
            cap->setCubeInfo(mydata.marker_size, mydata.marker_length, mydata.marker_margin, mydata.cube_count);
            cap->setThreshG(mydata.threshold);

            mydata.Log("CAMERA OPENED (camera_id=" + QString::number(mydata.camera_id) +
                ", marker_size=" + QString::number(mydata.marker_size) +
                ", marker_length=" + QString::number(mydata.marker_length) +
                ", marker_margin=" + QString::number(mydata.marker_margin) +
                ", cube_count=" + QString::number(mydata.cube_count) +
                ", threshold=" + QString::number(mydata.threshold) +
                ")");
            ct->open(cap);
        }
    });

    QObject::connect(w.ui.ssopen, &QPushButton::clicked, [&]() {
        if (mydata.server_status) {
            w.setServerStatus(false);
            mydata.Log("SERVER CLOSED");
        }
        else {
            w.setServerStatus(true);
            mydata.Log("SERVER OPENED (server_port=" + QString::number(mydata.server_port) + ")");
        }
    });

    w.move(0, 0);
    w.show();
    w.setFixedSize(w.size());

    int rnt = a.exec();

    mydata.saveConfig();

    ct->requestInterruption();
    ct->wait();

    if (ct->getCameraFlag()) delete cap;
    delete ct;
    
    return rnt;
}
