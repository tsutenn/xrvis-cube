#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_gui.h"

//#include "gui.h"
//#include "ca.h"
#include "CaThread.h"

class gui : public QMainWindow
{
    Q_OBJECT

public:
    gui(QWidget *parent = nullptr);
    ~gui();

    Ui::guiClass ui;

    int camera_id;
    int threshold_value;

    int cube_count;
    int marker_size;
    int marker_margin;

    bool cameraStatus = false;

    void setCameraStatus(bool status);
};
