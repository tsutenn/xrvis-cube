#include "msg.h"

void msg::saveConfig()
{
    QJsonObject json;
    json["camera id"] = this->camera_id;
    json["threshold"] = this->threshold;

    json["marker size"] = this->marker_size;
    json["marker length"] = this->marker_length;
    json["marker margin"] = this->marker_margin;
    json["cube count"] = this->cube_count;

    json["server port"] = this->server_port;

    QJsonDocument doc(json);

    QFile file("config.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << doc.toJson();
        file.close();
    }
}

bool msg::loadConfig()
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

            return true;
        }
    }
    
    return false;
}

bool msg::loadMarkerList(QString& path)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int cnt = 0;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (cnt % 6 == 0) {

            }
            QStringList parts = line.split(',');
        }

        file.close();
    }
    return false;
}
