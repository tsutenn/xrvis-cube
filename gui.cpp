#include "gui.h"

gui::gui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    this->threshold_value = 120;
    this->camera_id = 0;

    this->cube_count = 0;
    this->marker_size = 0;
    this->marker_margin = 0;

    ui.cameraid->setValue(this->camera_id);
    ui.threshold->setValue(this->threshold_value);
    ui.thresholdslider->setValue(this->threshold_value);

    ui.cubecount->setValue(cube_count);
    ui.cubesize->setValue(marker_size);
    ui.cubemargin->setValue(marker_margin);
}

gui::~gui()
{}

void gui::setCameraStatus(bool status) {
    ui.cameraid->setReadOnly(status);
    ui.cubecount->setReadOnly(status);
    ui.cubesize->setReadOnly(status);
    ui.cubemargin->setReadOnly(status);

    ui.cameraopen->setText(status ? "CLOSE" : "OPEN");

    this->cameraStatus = status;
}