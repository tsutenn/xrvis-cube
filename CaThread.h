#pragma once
#include <QThread>
#include <qlabel.h>

#include "ca.h"
#include "msg.h"

class CaThread :
    public QThread
{
    Q_OBJECT

public:
    CaThread(msg* data, QLabel* frameLabel, QLabel* binaryLabel, QLabel* edgeLabel, QObject* parent = nullptr) : QThread(parent) {
        this->cap = nullptr;
        this->mydata = data;
        
        this->frameLabel = frameLabel;
        this->binaryLabel = binaryLabel;
        this->edgeLabel = edgeLabel;

        this->outputLabel = nullptr;
    }

    void open(ca* cap) {
        this->cap = cap;
        this->cameraFlag = true;
    }

    void close() {
        this->cameraFlag = false;
        while (cap->LoopBlock());
        delete this->cap;
    }

    bool getCameraFlag() {
        return this->cameraFlag;
    }

    void SetMsg(msg * mydata) {
        this->mydata = mydata;
    }

    void setOutput(QLabel* outputLabel) {
        this->outputLabel = outputLabel;
    }

protected:
    void run() override {
        mydata->Log("cv thread started");
        while (!isInterruptionRequested()) {
            if (cameraFlag) {
                cap->fun();

                QImage image(cap->getFrame()->data, 
                    cap->getFrame()->cols, 
                    cap->getFrame()->rows, 
                    cap->getFrame()->step, 
                    QImage::Format_RGB888);
                image = image.rgbSwapped(); // BGR to RGB
                frameLabel->setPixmap(QPixmap::fromImage(image));

                QImage imageBin(cap->getBinaryFrame()->data, 
                    cap->getBinaryFrame()->cols, 
                    cap->getBinaryFrame()->rows, 
                    cap->getBinaryFrame()->step, 
                    QImage::Format_Grayscale8);
                binaryLabel->setPixmap(QPixmap::fromImage(imageBin));

                QImage imageEdge(cap->getEdgeFrame()->data, 
                    cap->getEdgeFrame()->cols, 
                    cap->getEdgeFrame()->rows, 
                    cap->getEdgeFrame()->step, 
                    QImage::Format_Grayscale8);
                edgeLabel->setPixmap(QPixmap::fromImage(imageEdge));

                if (cap->outputImages.size() > 0) {
                    QImage imageOut(cap->outputImages[0].data,
                        cap->outputImages[0].cols,
                        cap->outputImages[0].rows,
                        cap->outputImages[0].step,
                        QImage::Format_Grayscale8);
                    outputLabel->setPixmap(QPixmap::fromImage(imageOut));

                    QString msg = "";
                    for (int i = 0; i < cap->getMarkerSize(); i++) {
                        for (int j = 0; j < cap->getMarkerSize(); j++) {
                            msg += QString::number(cap->outputMarkers[0].At(j, i));
                            msg += ",";
                        }
                        msg += "\n";
                    }
                    msg += "\n";

                    mydata->Log(msg);
                }
            }
        }
    }

    bool cameraFlag = false;
    msg* mydata;

    ca* cap;
    QLabel* frameLabel;
    QLabel* binaryLabel;
    QLabel* edgeLabel;
    QLabel* outputLabel;
};
