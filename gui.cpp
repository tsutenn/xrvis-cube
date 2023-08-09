#include "gui.h"

gui::gui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    this->threshold = 120;
    this->camera_id = 0;

    this->marker_size = 0;
    this->marker_length = 0;
    this->marker_margin = 0;
    this->cube_count = 0;

    ui.cameraid->setValue(this->camera_id);
    ui.threshold->setValue(this->threshold);
    ui.thresholdslider->setValue(this->threshold);

    ui.markersize->setValue(marker_size);
    ui.markerlength->setValue(marker_length);
    ui.markermargin->setValue(marker_margin);
    ui.cubecount->setValue(cube_count);

    ui.LogPanel->setStyleSheet("background: transparent;");
    QPalette palette = ui.LogPanel->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    ui.LogPanel->setPalette(palette);
}

gui::~gui()
{}

void gui::setCameraStatus(bool status) {
    ui.cameraid->setReadOnly(status);
    
    ui.markersize->setReadOnly(status);
    ui.markerlength->setReadOnly(status);
    ui.markermargin->setReadOnly(status);
    ui.cubecount->setReadOnly(status);

    ui.cameraopen->setText(status ? "CLOSE" : "OPEN");

    this->camera_id = ui.cameraid->value();
    
    this->marker_size = ui.markersize->value();
    this->marker_length = ui.markerlength->value();
    this->marker_margin = ui.markermargin->value();
    this->cube_count = ui.cubecount->value();

    this->threshold = ui.threshold->value();

    this->cameraStatus = status;
}

void gui::setThreshold(int value)
{
    this->threshold = value;
    this->ui.thresholdslider->setValue(value);
    this->ui.threshold->setValue(value);
}
