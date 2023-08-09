#pragma once
#include <QThread>
#include <qlabel.h>
#include <qdebug.h>

#include "ca.h"
#include "MyLog.h"

class CaThread :
    public QThread
{
    Q_OBJECT

public:
    ca * cap;
    QLabel * frameLabel;
    QLabel * adptThrLabel;
    QLabel* edgeLabel;

    CaThread(QLabel* frameLabel, QLabel* adptThrLabel, QLabel* edgeLabel, QObject* parent = nullptr) : QThread(parent) {
        this->cap = nullptr;
        this->frameLabel = frameLabel;
        this->adptThrLabel = adptThrLabel;
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

    void setLog(MyLog* log) {
        this->mlog = log;
    }

protected:
    void run() override {
        while (!isInterruptionRequested()) {
            if (cameraFlag) {
                cap->fun();

                QImage image(cap->getFrame()->data, cap->getFrame()->cols, cap->getFrame()->rows, cap->getFrame()->step, QImage::Format_RGB888);
                image = image.rgbSwapped(); // BGR to RGB
                frameLabel->setPixmap(QPixmap::fromImage(image));

                QImage imageThr(cap->getAdptThr()->data, cap->getAdptThr()->cols, cap->getAdptThr()->rows, cap->getAdptThr()->step, QImage::Format_Grayscale8);
                adptThrLabel->setPixmap(QPixmap::fromImage(imageThr));

                QImage imageEdge(cap->getEdges()->data, cap->getEdges()->cols, cap->getEdges()->rows, cap->getEdges()->step, QImage::Format_Grayscale8);
                edgeLabel->setPixmap(QPixmap::fromImage(imageEdge));

                mlog->Log(QString::number(cap->getDetectedCount()));
            }
        }
    }

    bool cameraFlag = false;
    MyLog * mlog;
};

