#pragma once

#include <iostream>
#include <cstring>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SS_BUFFER_SIZE 10240

class ss
{
public:
	ss(int port);
	int start();
	int loop(const char* message);
	void close();

protected:
	int PORT;
	SOCKET serverSocket;
};

