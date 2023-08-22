#pragma once

#include <qobject.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qfile.h>
#include "Cube.h"

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

    int base_cube_id;

    std::vector<float> camera_position;
    std::vector<float> camera_rotation;

    bool camera_status = false;
    bool server_status = false;

    std::vector<Cube> cubes;

    Cube base_cube;
    std::vector<Cube> cube_list;

    QString path;

    msg(QObject* parent = nullptr);
    ~msg();

    void SaveConfig();
    bool LoadConfig();
    bool LoadMarkerList(const QString& path);

    bool SetBaseCube(std::vector<Cube>& cube_list, Cube& base_cube);

signals:
    void Log(const QString& message);
};

