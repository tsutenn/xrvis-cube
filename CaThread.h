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

    ~CaThread() {
        raw.release();
        grayFrame.release();
        binaryFrame.release();
        edgeFrame.release();
    }

    void open(ca* cap) {
        this->cap = cap;
        this->cameraFlag = true;
    }

    void close() {
        this->cameraFlag = false;
        while (cap->LoopBlock() || loopBlock);
        delete this->cap;

        outputLabel->setText("OUTPUT IMAGE");
        frameLabel->setText(" ");
        binaryLabel->setText(" ");
        edgeLabel->setText(" ");

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
    /*
    void run() override {
        mydata->Log("cv thread started");
        while (!isInterruptionRequested()) {
            if (cameraFlag) {
                cap->Fun();

                QImage image(cap->GetFrame()->data, 
                    cap->GetFrame()->cols,
                    cap->GetFrame()->rows,
                    cap->GetFrame()->step,
                    QImage::Format_RGB888);
                image = image.rgbSwapped(); // BGR to RGB
                frameLabel->setPixmap(QPixmap::fromImage(image));

                QImage imageBin(cap->GetBinaryFrame()->data, 
                    cap->GetBinaryFrame()->cols,
                    cap->GetBinaryFrame()->rows,
                    cap->GetBinaryFrame()->step,
                    QImage::Format_Grayscale8);
                binaryLabel->setPixmap(QPixmap::fromImage(imageBin));

                QImage imageEdge(cap->GetEdgeFrame()->data, 
                    cap->GetEdgeFrame()->cols,
                    cap->GetEdgeFrame()->rows,
                    cap->GetEdgeFrame()->step,
                    QImage::Format_Grayscale8);
                edgeLabel->setPixmap(QPixmap::fromImage(imageEdge));

                if (cap->outputMarkers.size() > 0) {
                    QImage imageOut(cap->outputMarkers[0].image.data,
                        cap->outputMarkers[0].image.cols,
                        cap->outputMarkers[0].image.rows,
                        cap->outputMarkers[0].image.step,
                        QImage::Format_Grayscale8);
                    outputLabel->setPixmap(QPixmap::fromImage(imageOut));

                    QString msg = "";
                    for (int i = 0; i < cap->GetMarkerSize(); i++) {
                        for (int j = 0; j < cap->GetMarkerSize(); j++) {
                            msg += QString::number(cap->outputMarkers[0].At(j, i));
                            msg += ",";
                        }
                        msg += "\n";
                    }
                    msg += "\n";

                    mydata->Log(msg);
                }
                else {
                    outputLabel->setText("OUTPUT IMAGE");
                }
            }
        }
    }
    */
    // /*
    void run() override {
        mydata->Log("cv thread started");
        while (!isInterruptionRequested()) {
            if (cameraFlag) {
                loopBlock = true;

                cap->GenerateFramesFromCapture(&raw, &grayFrame, &binaryFrame, &edgeFrame);
                auto detected_markers = cap->ExtractMarkersFromFrame(grayFrame, binaryFrame);
                // auto detected_cube_ids = cap->DetectedCubeId(detected_markers, 1);
                auto detected_cubes = cap->GenerateCubes(detected_markers, 1);

                QImage image(raw.data, raw.cols, raw.rows, raw.step, QImage::Format_RGB888);
                image = image.rgbSwapped(); // BGR to RGB
                frameLabel->setPixmap(QPixmap::fromImage(image));

                QImage imageBin(binaryFrame.data, binaryFrame.cols, binaryFrame.rows, binaryFrame.step, QImage::Format_Grayscale8);
                binaryLabel->setPixmap(QPixmap::fromImage(imageBin));

                QImage imageEdge(edgeFrame.data, edgeFrame.cols, edgeFrame.rows, edgeFrame.step, QImage::Format_Grayscale8);
                edgeLabel->setPixmap(QPixmap::fromImage(imageEdge));

                if (detected_markers.size() > 0) {
                    QImage imageOut(detected_markers[0].image.data, detected_markers[0].image.cols, detected_markers[0].image.rows, detected_markers[0].image.step, QImage::Format_Grayscale8);
                    this->outputLabel->setPixmap(QPixmap::fromImage(imageOut));
                }
                else {
                    outputLabel->setText("OUTPUT IMAGE");
                }

                if (detected_cubes.size() > 0) {
                    QString result = "detected cube:";
                    for (int i = 0; i < detected_cubes.size(); i++) {
                        result += " " + QString::number(detected_cubes[i].GetId()) + ",";
                    }
                    mydata->Log(result);
                }

                loopBlock = false;
                cap->Wait(40);
            }
        }
    }
    // */
    bool cameraFlag = false;
    bool loopBlock = false;
    msg* mydata;

    ca* cap;
    QLabel* frameLabel;
    QLabel* binaryLabel;
    QLabel* edgeLabel;
    QLabel* outputLabel;

    cv::Mat raw;
    cv::Mat grayFrame;
    cv::Mat binaryFrame;
    cv::Mat edgeFrame;
};
