#include "ss.h"

ss::ss(int port)
{
    PORT = port;
}

int ss::start()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return -1;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket." << std::endl;
        WSACleanup();
        return -2;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return -3;
    }

    std::cout << "Server listening on port " << PORT << std::endl;
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

void ss::close()
{
    closesocket(serverSocket);
    WSACleanup();
}
