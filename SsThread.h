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

    bool open(int port) {
        try {
            this->server = new ss(port);
            qDebug() << this->server->start();
            serverFlag = true;
            ipv4_address = QString::fromUtf8(this->server->getIPV4());
        }
        catch (const std::exception& e) {
            serverFlag = false;
            mydata->Log("\nOpen socket server with error:");
            mydata->Log(e.what());
        }

        return serverFlag;
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

signals:
    void ThrowError(QString);

protected:
    void run() override {
        while (!isInterruptionRequested()) {
            if (serverFlag) {
                try {
                    sockaddr_in clientAddress = server->receive();
                    mydata->server_loop = true;

                    // qDebug() << "received request: ";

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
                        message += QString::number(detected_cubes[i].transform.position[0] + mydata->base_position[0]) + ",";
                        message += QString::number(detected_cubes[i].transform.position[1] + mydata->base_position[1]) + ",";
                        message += QString::number(detected_cubes[i].transform.position[2] + mydata->base_position[2]) + ",";
                        message += QString::number(detected_cubes[i].transform.rotation[0]) + ",";
                        message += QString::number(detected_cubes[i].transform.rotation[1]) + ",";
                        message += QString::number(detected_cubes[i].transform.rotation[2]) + ",";
                        message += QString::number(detected_cubes[i].transform.rotation[3]) + "\n";
                    }
                    // qDebug() << message << endl;
                    server->response(clientAddress, message.toUtf8());

                    mydata->server_loop = false;
                }
                catch (const std::exception& e) {
                    mydata->server_loop = false;
                    server->close();
                    ThrowError(e.what());

                    QThread::msleep(500);
                }
            }
        }
    }

    msg* mydata;
    ss* server;
    bool serverFlag = false;

    QString ipv4_address;
};

