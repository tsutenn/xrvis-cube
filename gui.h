#pragma once

#include <QtWidgets/QMainWindow>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qfile.h>
#include <qtextstream.h>

#include "ui_gui.h"
#include "msg.h"

class gui : public QMainWindow
{
    Q_OBJECT

public:
    gui(msg* data, QWidget *parent = nullptr);
    ~gui();

    Ui::guiClass ui;

    msg* mydata;

    void setCameraStatus(bool status);
    void setThreshold(int value);

    void setServerStatus(bool status);

public slots:
    void Log(const QString& message);
};
