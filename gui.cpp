#include "gui.h"

gui::gui(msg* data, QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    mydata = data;

    if (mydata->LoadConfig()) {
        ui.cameraid->setValue(mydata->camera_id);
        ui.threshold->setValue(mydata->threshold);
        ui.thresholdslider->setValue(mydata->threshold);

        ui.markersize->setValue(mydata->marker_size);
        ui.markerlength->setValue(mydata->marker_length);
        ui.markermargin->setValue(mydata->marker_margin);
        ui.cubecount->setValue(mydata->cube_count);

        ui.ssport->setValue(mydata->server_port);

        ui.pathbox->setText(mydata->path);

        ui.basecubeid->setValue(mydata->base_cube_id);

        ui.camposx->setValue(mydata->camera_position[0]);
        ui.camposy->setValue(mydata->camera_position[1]);
        ui.camposz->setValue(mydata->camera_position[2]);

        ui.camrotx->setValue(mydata->camera_rotation[0]);
        ui.camroty->setValue(mydata->camera_rotation[1]);
        ui.camrotz->setValue(mydata->camera_rotation[2]);
    }
    else {
        mydata->threshold = ui.threshold->value();
        mydata->camera_id = ui.cameraid->value();

        mydata->marker_size = ui.markersize->value();
        mydata->marker_length = ui.markerlength->value();
        mydata->marker_margin = ui.markermargin->value();

        mydata->server_port = ui.ssport->value();

        mydata->path = ui.pathbox->text();
    }

    ui.LogPanel->setStyleSheet("background: transparent;");
    QPalette palette = ui.LogPanel->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    ui.LogPanel->setPalette(palette);
}

gui::~gui()
{}

void gui::SetCameraStatus(bool status) {
    ui.cameraid->setReadOnly(status);
    
    ui.markersize->setReadOnly(status);
    ui.markerlength->setReadOnly(status);
    ui.markermargin->setReadOnly(status);

    ui.basecubeid->setReadOnly(status);

    ui.cameraopen->setText(status ? "CLOSE" : "OPEN");

    mydata->camera_id = ui.cameraid->value();
    
    mydata->marker_size = ui.markersize->value();
    mydata->marker_length = ui.markerlength->value();
    mydata->marker_margin = ui.markermargin->value();

    mydata->base_cube_id = ui.basecubeid->value();

    mydata->camera_status = status;
}

void gui::SetThreshold(int value)
{
    mydata->threshold = value;
    this->ui.thresholdslider->setValue(value);
    this->ui.threshold->setValue(value);
}

void gui::SetServerStatus(bool status)
{
    ui.ssport->setReadOnly(status);
    ui.ssopen->setText(status ? "CLOSE" : "OPEN");
    mydata->server_port = ui.ssport->value();
    mydata->server_status = status;
}

void gui::Log(const QString & message) {
    this->ui.LogPanel->appendPlainText(message);
}
