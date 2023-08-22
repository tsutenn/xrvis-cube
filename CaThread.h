#pragma once
#include <qthread.h>
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
        while (mydata->camera_loop);
        delete this->cap;

        outputLabel->setText("OUTPUT IMAGE");
        frameLabel->setText(" ");
        binaryLabel->setText(" ");
        edgeLabel->setText(" ");

        mydata->detected_cubes.clear();
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
                mydata->camera_loop = true;
                while (mydata->write_loop);

                cap->GenerateFramesFromCapture(raw, grayFrame, binaryFrame, edgeFrame);
                auto detected_markers = cap->ExtractMarkersFromFrame(grayFrame, binaryFrame);
                
                cap->GenerateBaseCube(mydata->base_cube, detected_markers, 1);
                mydata->detected_cubes = cap->GenerateCubes(mydata->cube_list, mydata->base_cube, detected_markers, 1);

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

                if (mydata->detected_cubes.size() > 0) {
                    QString result = "detected cubes: (" + QString::number(mydata->detected_cubes.size()) + ")\n";
                    for (int i = 0; i < mydata->detected_cubes.size(); i++) {
                        result += "#" + QString::number(mydata->detected_cubes[i].GetId()) + "\n";
                        result += QString::fromUtf8(mydata->detected_cubes[i].GetTransformInString()) + "\n";
                        result += QString::fromUtf8(mydata->detected_cubes[i].GetRotationInString()) + "\n";
                    }
                    mydata->Log(result);
                }

                mydata->camera_loop = false;
                cap->Wait(34);
            }
        }
    }

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
