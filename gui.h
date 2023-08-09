#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_gui.h"

class gui : public QMainWindow
{
    Q_OBJECT

public:
    gui(QWidget *parent = nullptr);
    ~gui();

    Ui::guiClass ui;

    int camera_id;
    int threshold;

    int marker_size;
    double marker_length;
    double marker_margin;
    int cube_count;

    bool cameraStatus = false;

    void setCameraStatus(bool status);
    void setThreshold(int value);

public slots:
    void Log(const QString& message) {
        this->ui.LogPanel->appendPlainText(message);
    }
};
