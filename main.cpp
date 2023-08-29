#include <QtWidgets/QApplication>

#include "ca.h"
#include "ss.h"

#include "gui.h"
#include "msg.h"
#include "CaThread.h"
#include "SsThread.h"

#define CAMERA_ID 0

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    msg mydata;
    gui w(&mydata);

    QObject::connect(&mydata, &msg::Log, &w, &gui::Log);
    QObject::connect(w.ui.cubesload, &QPushButton::clicked, [&] {
        if (mydata.LoadMarkerList(w.ui.pathbox->text())) {
            mydata.path = w.ui.pathbox->text();
            w.ui.cubecount->setValue(mydata.cube_count);
            mydata.Log("Successfully load cube info at " + mydata.path + " (cube count: " + QString::number(mydata.cubes.size()) + ")");
        }
        else {
            mydata.Log("Load file failed");
        }
    });

    CaThread* ct;
    SsThread* st;

    ct = new CaThread(&mydata, w.ui.rawlabel, w.ui.binlabel, w.ui.edge);
    ct->start();
    ct->setOutput(w.ui.outputlabel);

    st = new SsThread(&mydata);
    st->start();

    QObject::connect(ct, &CaThread::ThrowError, [&](QString message) {
        mydata.Log("\nCamera has closed with an error:");
        mydata.Log(message);
        mydata.cube_list.clear();

        mydata.Log("\nTrying to re-open camera...");
        
        std::vector<Cube> cube_list;
        Cube base_cube;
        mydata.SetBaseCube(cube_list, base_cube);

        ct->open(mydata.camera_id, mydata.cam_matrix, mydata.dist_coeff,
            mydata.marker_size, mydata.marker_length, mydata.marker_margin,
            cube_list, base_cube);
        ct->SetCaThreshold(mydata.threshold);
    });
    
    QObject::connect(st, &SsThread::ThrowError, [&](QString message) {
        mydata.Log("\Socket server has closed with an error:");
        mydata.Log(message);

        mydata.Log("\nTrying to re-open server...");

        st->open(mydata.server_port);
        w.ui.ssip->setText(st->ipv4Address());
    });

    QObject::connect(w.ui.thresholdslider, &QSlider::valueChanged, [&](int value) {
        if (ct->getCameraFlag()) {
            ct->SetCaThreshold(value);
            w.SetThreshold(value);
        }
        else {
            w.ui.threshold->setValue(value);
        }
    });

    QObject::connect(w.ui.threshold, QOverload<int>::of(&QSpinBox::valueChanged), [&](int value) {
        if (ct->getCameraFlag()) {
            ct->SetCaThreshold(value);
            w.SetThreshold(value);
        }
        else {
            w.ui.thresholdslider->setValue(value);
        }
    });

    QObject::connect(w.ui.cameraopen, &QPushButton::clicked, [&]() {
        if (mydata.camera_status) {
            w.SetCameraStatus(false);

            if(ct->getCameraFlag())
                ct->close();
            mydata.cube_list.clear();

            mydata.Log("\nCAMERA CLOSED");
        }

        else {
            w.SetCameraStatus(true);

            std::vector<Cube> cube_list;
            Cube base_cube;
            mydata.SetBaseCube(cube_list, base_cube);

            mydata.Log("\nCAMERA OPENED (camera_id=" + QString::number(mydata.camera_id) +
                ", marker_size=" + QString::number(mydata.marker_size) +
                ", marker_length=" + QString::number(mydata.marker_length) +
                ", marker_margin=" + QString::number(mydata.marker_margin) +
                ", cube_count=" + QString::number(mydata.cube_count) +
                ", threshold=" + QString::number(mydata.threshold) +
                ")");

            ct->open(mydata.camera_id, mydata.cam_matrix, mydata.dist_coeff,
                mydata.marker_size, mydata.marker_length, mydata.marker_margin,
                cube_list, base_cube);
            ct->SetCaThreshold(mydata.threshold);
        }
    });

    QObject::connect(w.ui.ssopen, &QPushButton::clicked, [&]() {
        if (mydata.server_status) {
            w.SetServerStatus(false);
            st->close();
            mydata.Log("\nSERVER CLOSED");
        }
        else {
            w.SetServerStatus(true);
            st->open(mydata.server_port);
            w.ui.ssip->setText(st->ipv4Address());

            mydata.Log("\nSERVER OPENED (ip=" + st->ipv4Address() + "server_port=" + QString::number(mydata.server_port) + ")");
        }
    });

    w.move(0, 0);
    w.show();
    w.setFixedSize(w.size());

    w.ui.cubesload->clicked();

    int rnt = a.exec();

    mydata.SaveConfig();

    ct->requestInterruption();
    st->requestInterruption();

    if (ct->getCameraFlag()) ct->close();
    if (st->getServerFlag()) st->close();

    ct->wait();
    st->wait();

    delete ct;
    delete st;

    return rnt;
}
