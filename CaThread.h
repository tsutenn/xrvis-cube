#pragma once
#include <QThread>
#include <qlabel.h>

#include "ca.h"
#include "GuiLog.h"

class CaThread :
    public QThread
{
    Q_OBJECT

public:
    ca * cap;
    QLabel* frameLabel;
    QLabel* binaryLabel;
    QLabel* edgeLabel;

    CaThread(QLabel* frameLabel, QLabel* binaryLabel, QLabel* edgeLabel, QObject* parent = nullptr) : QThread(parent) {
        this->cap = nullptr;
        this->glog = nullptr;
        
        this->frameLabel = frameLabel;
        this->binaryLabel = binaryLabel;
        this->edgeLabel = edgeLabel;
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

    void SetLog(GuiLog * glog) {
        this->glog = glog;
    }

protected:
    void run() override {
        glog->Log("cv thread started");
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

                glog->Log(QString::number(cap->getDetectedCount()));
            }
        }
    }

    bool cameraFlag = false;
    GuiLog * glog;
};

