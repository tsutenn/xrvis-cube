#pragma once
#include <qthread.h>

#include "ss.h"
#include "msg.h"

class SsThread :
    public QThread
{
    Q_OBJECT

public:
    SsThread(msg* data, QObject* parent = nullptr) : QThread(parent) {
        this->mydata = data;
        this->server = nullptr;
    }

    void open(ss* server) {
        this->server = server;
        qDebug() << this->server->start();
        this->serverFlag = true;
        ipv4_address = QString::fromUtf8(this->server->getIPV4());
    }

    bool getServerFlag() {
        return serverFlag;
    }

    void close() {
        this->serverFlag = false;
        while (mydata->server_loop);
        delete this->server;
    }

    QString ipv4Address() {
        return ipv4_address;
    }

protected:
    void run() override {
        while (!isInterruptionRequested()) {
            if (serverFlag) {
                sockaddr_in clientAddress = server->receive();
                mydata->server_loop = true;

                qDebug() << "received request: ";

                mydata->write_loop = true;
                while (mydata->camera_loop);
                std::vector<Cube> detected_cubes;
                for (int i = 0; i < mydata->detected_cubes.size(); i++) {
                    detected_cubes.push_back(mydata->detected_cubes[i]);
                }
                mydata->write_loop = false;

                QString message = "";
                for (int i = 0; i < detected_cubes.size(); i++) {
                    message += QString::number(detected_cubes[i].GetId()) + ",";
                    message += QString::number(detected_cubes[i].GetTranslation()[0]) + ",";
                    message += QString::number(detected_cubes[i].GetTranslation()[1]) + ",";
                    message += QString::number(detected_cubes[i].GetTranslation()[2]) + ",";
                    message += QString::number(detected_cubes[i].GetRotation()[0]) + ",";
                    message += QString::number(detected_cubes[i].GetRotation()[1]) + ",";
                    message += QString::number(detected_cubes[i].GetRotation()[2]) + ",";
                    message += QString::number(detected_cubes[i].GetRotation()[3]) + "\n";
                }
                qDebug() << message << endl;
                server->response(clientAddress, message.toUtf8());

                mydata->server_loop = false;
            }
        }
    }

    msg* mydata;
    ss* server;
    bool serverFlag = false;

    QString ipv4_address;
};

