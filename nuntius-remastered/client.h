#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <print>
#include <format>

#include "message.h"

class Client {
private:
    std::string host;
    int port;
    SOCKET clientSocket;
    sockaddr_in serverAddress = {};

public:
    Client(std::string _host = "127.0.0.1", int _port = 8000): host(_host), port(_port) {}

    void init() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::string errMessage = "WSAStartup failed with error: " + WSAGetLastError();
            throw std::runtime_error(errMessage);
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            std::string errMessage = "Socket creation failed with error: " + WSAGetLastError();
            throw std::runtime_error(errMessage);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
            std::string errMessage =  "Invalid address/ Address not supported";
            closesocket(clientSocket);
            throw std::runtime_error(errMessage);
        }
    }

    void connectToServer(std::string username) {
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
            std::string errMessage = "Connection failed with error: " + WSAGetLastError();
            closesocket(clientSocket);
            throw std::runtime_error(errMessage);
        }

        // ConnectMessagePayload payload = { username };
        // sendConnectMessage(clientSocket, payload);
        // std::print("Connected to the server ;)\n");

        // while(true) {

        // }
    }
};