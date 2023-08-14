#pragma once

#include <qobject.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qfile.h>

class msg : public QObject
{
	Q_OBJECT

public:
    int camera_id;
    int threshold;

    int marker_size;
    double marker_length;
    double marker_margin;
    int cube_count;

    int server_port;

    bool camera_status = false;
    bool server_status = false;

    void saveConfig();
    bool loadConfig();
    bool loadMarkerList(QString& path);

signals:
    void Log(const QString& message);
};

