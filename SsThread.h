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

    void SetSocketServer(ss* server) {
        this->server = server;
        this->server->start();
        this->serverFlag = true;
    }

    bool GetServerFlag() {
        return serverFlag;
    }

protected:
    void run() override {
        while (1) {
            if (serverFlag) {
                loopBlock = true;

                QString message = "";
                server->loop(message.toUtf8());

                loopBlock = true;
            }
        }
    }

    msg* mydata;
    ss* server;
    bool serverFlag = false;
    bool loopBlock = true;
};

