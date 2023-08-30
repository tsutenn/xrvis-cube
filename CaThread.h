#pragma once
#include <qthread.h>
#include <qlabel.h>
#include <qtablewidget.h>

#include "ca.h"
#include "msg.h"

class CaThread :
    public QThread
{
    Q_OBJECT

public:
    CaThread(msg* data, QLabel* frameLabel, QLabel* binaryLabel, QTableWidget* tableWidget, QLabel* outputLabel, QObject* parent = nullptr) : QThread(parent) {
        this->cap = nullptr;
        this->mydata = data;
        
        this->frameLabel = frameLabel;
        this->binaryLabel = binaryLabel;
        this->tableWidget = tableWidget;
        this->outputLabel = outputLabel;
    }

    ~CaThread() {
        raw.release();
        grayFrame.release();
        binaryFrame.release();
    }

    void open(ca* cap) {
        this->cap = cap;
        this->cameraFlag = true;
    }

    void open(int camera_id, std::vector<double> cam_matrix, std::vector<double> dist_coeff, 
        int marker_size, float marker_length, float marker_margin, std::vector<Cube> cube_list, Cube base_cube) {
        this->cap = new ca(camera_id, cam_matrix, dist_coeff);
        cap->SetCubeInfo(marker_size, marker_length, marker_margin);
        cap->SetCubeList(cube_list, base_cube);
        this->cameraFlag = true;
    }

    bool SetCaData(int marker_size, float marker_length, float marker_margin, std::vector<Cube> cube_list, Cube base_cube) {
        try {
            cap->SetCubeInfo(marker_size, marker_length, marker_margin);
            cap->SetCubeList(cube_list, base_cube);
            return true;
        }
        catch (const std::exception& e) {
            mydata->Log("\nSet camera data error:");
            mydata->Log(e.what());
        }
        return false;
    }

    void close() {
        this->cameraFlag = false;
        while (mydata->camera_loop);
        delete this->cap;

        outputLabel->setText("OUTPUT IMAGE");
        frameLabel->setText(" ");
        binaryLabel->setText(" ");

        mydata->detected_cubes.clear();
    }

    bool getCameraFlag() {
        return this->cameraFlag;
    }

    void SetMsg(msg * mydata) {
        this->mydata = mydata;
    }

    void SetCaThreshold(int value) {
        cap->SetThreshG(value);
    }

signals:
    void ThrowError(QString);

protected:    
    void run() override {
        mydata->Log("cv thread started");

        while (!isInterruptionRequested()) {
            if (cameraFlag) {
                try {
                    mydata->camera_loop = true;
                    while (mydata->write_loop);

                    cap->GenerateFramesFromCapture(raw, grayFrame, binaryFrame);
                    auto detected_markers = cap->ExtractMarkersFromFrame(grayFrame, binaryFrame);

                    auto base_markers = cap->GenerateBaseCube(mydata->base_cube, detected_markers, mydata->marker_min_distance);
                    mydata->detected_cubes = cap->GenerateCubes(mydata->cube_list, mydata->base_cube, detected_markers, mydata->marker_min_distance);

                    QImage image(raw.data, raw.cols, raw.rows, raw.step, QImage::Format_RGB888);
                    image = image.rgbSwapped();
                    frameLabel->setPixmap(QPixmap::fromImage(image));

                    QImage imageBin(binaryFrame.data, binaryFrame.cols, binaryFrame.rows, binaryFrame.step, QImage::Format_Grayscale8);
                    binaryLabel->setPixmap(QPixmap::fromImage(imageBin));

                    if (base_markers.size() > 0) {
                        QImage imageOut(base_markers[0].image.data, base_markers[0].image.cols, base_markers[0].image.rows, base_markers[0].image.step, QImage::Format_Grayscale8);
                        this->outputLabel->setPixmap(QPixmap::fromImage(imageOut));
                    }
                    else {
                        outputLabel->setText("NO BASECUBE");
                    }

                    for (int row = 0; row < tableWidget->rowCount(); ++row) {
                        QTableWidgetItem* item = tableWidget->item(row, 0);
                        bool cubeExists = false;

                        for (int i = 0; i < mydata->detected_cubes.size(); i++) {
                            if (item->text().toInt() == mydata->detected_cubes[i].GetId()) {
                                cubeExists = true;
                                break;
                            }
                        }
                        
                        if (!cubeExists) {
                            tableWidget->removeRow(row);
                        }
                    }

                    for (int i = 0; i < mydata->detected_cubes.size(); i++) {
                        bool cubeExists = false;
                        for (int row = 0; row < tableWidget->rowCount(); ++row) {
                            QTableWidgetItem* item = tableWidget->item(row, 0);
                            if (item->text().toInt() == mydata->detected_cubes[i].GetId()) {
                                cubeExists = true;

                                tableWidget->item(row, 1)->setText(QString::number(mydata->detected_cubes[i].transform.position[0]));
                                tableWidget->item(row, 2)->setText(QString::number(mydata->detected_cubes[i].transform.position[1]));
                                tableWidget->item(row, 3)->setText(QString::number(mydata->detected_cubes[i].transform.position[2]));
                                tableWidget->item(row, 4)->setText(QString::number(mydata->detected_cubes[i].transform.rotation[0]));
                                tableWidget->item(row, 5)->setText(QString::number(mydata->detected_cubes[i].transform.rotation[1]));
                                tableWidget->item(row, 6)->setText(QString::number(mydata->detected_cubes[i].transform.rotation[2]));
                                tableWidget->item(row, 7)->setText(QString::number(mydata->detected_cubes[i].transform.rotation[3]));

                                break;
                            }
                        }

                        if (!cubeExists) {
                            int row = tableWidget->rowCount();
                            tableWidget->insertRow(row);

                            QTableWidgetItem* newItem0 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].GetId()));
                            QTableWidgetItem* newItem1 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.position[0]));
                            QTableWidgetItem* newItem2 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.position[1]));
                            QTableWidgetItem* newItem3 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.position[2]));
                            QTableWidgetItem* newItem4 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.rotation[0]));
                            QTableWidgetItem* newItem5 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.rotation[1]));
                            QTableWidgetItem* newItem6 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.rotation[2]));
                            QTableWidgetItem* newItem7 = new QTableWidgetItem(QString::number(mydata->detected_cubes[i].transform.rotation[3]));

                            tableWidget->setItem(row, 0, newItem0);
                            tableWidget->setItem(row, 1, newItem1);
                            tableWidget->setItem(row, 2, newItem2);
                            tableWidget->setItem(row, 3, newItem3);
                            tableWidget->setItem(row, 4, newItem4);
                            tableWidget->setItem(row, 5, newItem5);
                            tableWidget->setItem(row, 6, newItem6);
                            tableWidget->setItem(row, 7, newItem7);
                        }
                    }

                    tableWidget->viewport()->update();

                    mydata->camera_loop = false;
                    cap->Wait(34);
                }

                catch (const std::exception& e) {
                    mydata->camera_loop = false;
                    close();
                    ThrowError(e.what());

                    QThread::msleep(500);
                }
            }
        }
    }

    bool cameraFlag = false;
    bool loopBlock = false;
    msg* mydata;

    ca* cap;
    QLabel* frameLabel;
    QLabel* binaryLabel;
    QTableWidget* tableWidget;
    QLabel* outputLabel;

    cv::Mat raw;
    cv::Mat grayFrame;
    cv::Mat binaryFrame;
};
