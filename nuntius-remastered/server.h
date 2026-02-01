#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <thread>
#include <print>
#include <format>

struct Message {
    std::string msg;
};

struct PrivateMessage : public Message {
    int clientID;
};

class Server {
private:
    int port;
    SOCKET serverSocket;
    std::vector<SOCKET> clients;

public:
    Server(int _port = 8000): port(_port) {
        this->init(port);
    }

    void init(int port) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::string errMessage = "WSAStartup failed with error: " + WSAGetLastError();
            MessageBoxA(NULL, errMessage.c_str(), "Server Creation Error", MB_OK | MB_ICONERROR);
            return;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::string errMessage = "Socket creation failed with error: " + WSAGetLastError();
            MessageBoxA(NULL, errMessage.c_str(), "Server Creation Error", MB_OK | MB_ICONERROR);
            return;
        }

        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            std::string errMessage = "Bind failed with error: " + WSAGetLastError();
            MessageBoxA(NULL, errMessage.c_str(), "Server Creation Error", MB_OK | MB_ICONERROR);
            closesocket(serverSocket);
            return;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::string errMessage = "Listen failed with error: " + WSAGetLastError();
            MessageBoxA(NULL, errMessage.c_str(), "Server Creation Error", MB_OK | MB_ICONERROR);
            closesocket(serverSocket);
            return;
        }

        std::print("Server is listen on port {}...", port);
    }
};