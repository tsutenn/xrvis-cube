#include "msg.h"

msg::msg(QObject* parent) : QObject(parent) {
    base_position = std::vector<float>(3);
}

msg::~msg()
{
}

void msg::SaveConfig()
{
    QJsonObject json;
    json["camera id"] = this->camera_id;
    json["threshold"] = this->threshold;

    json["marker size"] = this->marker_size;
    json["marker length"] = this->marker_length;
    json["marker margin"] = this->marker_margin;

    json["server port"] = this->server_port;

    json["path"] = this->path;

    json["base cube"] = this->base_cube_id;

    json["base x"] = this->base_position[0];
    json["base y"] = this->base_position[1];
    json["base z"] = this->base_position[2];

    json["marker min_distance"] = this->marker_min_distance;
    json["filter window size"] = this->filter_window_size;

    QString cam_matrix_str = "";
    for (int i = 0; i < this->cam_matrix.size(); i++) {
        cam_matrix_str += QString::number(cam_matrix[i]);
        if (i != this->cam_matrix.size()) {
            cam_matrix_str += ",";
        }
    }
    json["camera matrix"] = cam_matrix_str;

    QString dist_coeff_str = "";
    for (int i = 0; i < this->dist_coeff.size(); i++) {
        dist_coeff_str += QString::number(dist_coeff[i]);
        if (i != this->dist_coeff.size()) {
            dist_coeff_str += ",";
        }
    }
    json["distortion coefficients"] = dist_coeff_str;

    QJsonDocument doc(json);

    QFile file("config.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << doc.toJson();
        file.close();
    }
}

bool msg::LoadConfig()
{
    QFile file("config.json");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString jsonText = in.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8());

        if (!doc.isNull()) {
            QJsonObject json = doc.object();

            this->camera_id = json["camera id"].toInt();
            this->threshold = json["threshold"].toInt();

            this->marker_size = json["marker size"].toInt();
            this->marker_length = json["marker length"].toDouble();
            this->marker_margin = json["marker margin"].toDouble();
            this->cube_count = json["cube count"].toInt();

            this->server_port = json["server port"].toInt();

            this->path = json["path"].toString();

            this->base_cube_id = json["base cube"].toInt();

            this->base_position[0] = json["base x"].toDouble();
            this->base_position[1] = json["base x"].toDouble();
            this->base_position[2] = json["base x"].toDouble();

            this->marker_min_distance = json["marker min_distance"].toInt();
            this->filter_window_size = json["filter window size"].toInt();

            QString cam_matrix_str = json["camera matrix"].toString();
            QString dist_coeff_str = json["distortion coefficients"].toString();

            QStringList stringList1 = cam_matrix_str.split(",");
            for (const QString& str : stringList1) {
                bool conversionOk = false;
                double value = str.toDouble(&conversionOk);
                if (conversionOk) {
                    this->cam_matrix.push_back(value);
                }
            }

            QStringList stringList2 = dist_coeff_str.split(",");
            for (const QString& str : stringList2) {
                bool conversionOk = false;
                double value = str.toDouble(&conversionOk);
                if (conversionOk) {
                    this->dist_coeff.push_back(value);
                }
            }

            return true;
        }
    }
    
    return false;
}

bool msg::LoadMarkerList(const QString& path)
{
    cubes.clear();

    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int cnt = 0;
        int cube_id = -1;

        Log("start read file at " + path);

        std::vector<std::vector<int>> marker_data(6, std::vector<int>(marker_size * marker_size));

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();

            if (cnt % 7 == 0) {
                bool ok;
                cube_id = line.toInt(&ok);
            }
            else {
                QStringList parts = line.split(',');

                for (int i = 0; i < parts.size(); i++) {
                    bool ok;
                    marker_data[cnt % 7 - 1][i] = parts[i].toInt(&ok);
                    int value = parts[i].toInt(&ok);
                }

                if (cnt % 7 == 6) {
                    Cube cube(cube_id, marker_size, marker_data);
                    cube.SetFilterWindowSize(filter_window_size);
                    cubes.push_back(cube);
                }
            }
            cnt++;
        }

        file.close();
        this->cube_count = cubes.size();

        return true;
    }
    return false;
}

bool msg::SetBaseCube(std::vector<Cube>& cube_list, Cube& base_cube) {
    bool contain = false;
    cube_list.clear();

    for (int i = 0; i < cubes.size(); i++) {
        if (cubes[i].GetId() == base_cube_id) {
            this->base_cube = cubes[i];
            contain = true;
        }
        else {
            this->cube_list.push_back(cubes[i]);
        }
    }

    cube_list = this->cube_list;
    base_cube = this->base_cube;

    return contain;
}