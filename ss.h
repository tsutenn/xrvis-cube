#pragma once

#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define SS_BUFFER_SIZE 10240

class ss
{
public:
	ss(int port);
	~ss();
	int start();
	int loop(const char* message);
	void close();

	sockaddr_in receive();
	int response(sockaddr_in clientAddress, const char* message);

	const char* getIPV4();

protected:
	int PORT;
	SOCKET serverSocket;
};

