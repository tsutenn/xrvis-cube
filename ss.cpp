#include "ss.h"

ss::ss(int port)
{
    PORT = port;
}

ss::~ss()
{
    close();
}

int ss::start()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return -1;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        return -2;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        return -3;
    }

    return 1;
}

int ss::loop(const char* message)
{
    char buffer[SS_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    sockaddr_in clientAddress{};
    int clientAddressLength = sizeof(clientAddress);
    int bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

    if (bytesRead == SOCKET_ERROR) {
        std::cerr << "Failed to receive data." << std::endl;
        return -1;
    }

    int bytesSent = sendto(serverSocket, message, strlen(message), 0, (struct sockaddr*)&clientAddress, clientAddressLength);

    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "Failed to send response." << std::endl;
        return -2;
    }

    return 1;
}

sockaddr_in ss::receive() {
    char buffer[SS_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    sockaddr_in clientAddress{};
    int clientAddressLength = sizeof(clientAddress);
    int bytesRead = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddress, &clientAddressLength);

    return clientAddress;
}

int ss::response(sockaddr_in clientAddress, const char* message)
{
    int clientAddressLength = sizeof(clientAddress);

    int bytesSent = sendto(serverSocket, message, strlen(message), 0, (struct sockaddr*)&clientAddress, clientAddressLength);
    if (bytesSent == SOCKET_ERROR) {
        return -1;
    }

    return 1;
}



void ss::close()
{
    closesocket(serverSocket);
    WSACleanup();
}

const char* ss::getIPV4()
{
    char hostname[256];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct addrinfo hints;
        struct addrinfo* res;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;  // IPv4
        hints.ai_socktype = SOCK_STREAM;

        int result = getaddrinfo(hostname, NULL, &hints, &res);
        if (result == 0) {
            struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
            char* ip = inet_ntoa(addr->sin_addr);
            freeaddrinfo(res);

            return(ip);
        }
    }

    const char* result = "error";
    return result;
}
