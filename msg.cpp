#include "msg.h"

msg::msg(QObject* parent) : QObject(parent) {
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
    json["cube count"] = this->cube_count;

    json["server port"] = this->server_port;

    json["path"] = this->path;

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

            return true;
        }
    }
    
    return false;
}

bool msg::LoadMarkerList(const QString& path)
{
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
                    // Cube cube(cube_id, marker_size, marker_data);
                    cubes.push_back(Cube(cube_id, marker_size, marker_data));
                }
            }
            cnt++;
        }

        file.close();
        return true;
    }
    return false;
}
