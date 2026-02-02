#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <thread>
#include <print>
#include <format>
#include <map>
#include "message.h"

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
    std::map<std::string, SOCKET*> clients;

public:
    Server(int _port = 8000): port(_port) {
        this->init(port);
    }

    void init(int port) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::string errMessage = "WSAStartup failed with error: " + WSAGetLastError();
            throw errMessage;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::string errMessage = "Socket creation failed with error: " + WSAGetLastError();
            throw errMessage;
        }

        sockaddr_in serverAddress = {};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        
        if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
            std::string errMessage = "Bind failed with error: " + WSAGetLastError();
            closesocket(serverSocket);
            throw errMessage;
        }

        if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::string errMessage = "Listen failed with error: " + WSAGetLastError();
            closesocket(serverSocket);
            throw errMessage;
        }

        std::print("Server is listen on port {}...\n", port);
        std::thread serverThread([this]() { this->serverMainThread(); });
        serverThread.detach();
    }

    std::string receiveClientMessage(SOCKET &clientSocket) {
        char buffer[1024] = { 0 };
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        return message;
    }

    void serverMainThread() {
        while(true) {
            sockaddr_in clientAddress = {};
            int clientAddressLen = sizeof(clientAddress);
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressLen);
            if (clientSocket == INVALID_SOCKET) {
                std::string errMessage = "Accept client failed with error: " + WSAGetLastError();
                std::print("{}\n", errMessage);
                continue;
            }

            try {
                std::string clientUsername = receiveClientMessage(clientSocket);
                addNewClient(clientUsername, clientAddress, clientSocket);
            } catch(std::string err) {
                std::print("{}\n", err);
                continue;
            }
        }
    }

    bool isUsernameInUse(std::string username) {
        auto it = clients.find(username);
        return it != clients.end();
    }

    void handleClient(SOCKET &socket) {
        while(true) {
            try {
                Header header;
                readMessageHeader(socket, header);

                switch(header.type) {
                    case CONNECT: {
                        ConnectMessagePayload payload;
                        readConnectMessage(socket, header, payload);

                        std::print("Received connection request with username: {}", payload.username);
                    }
                    default: {
                        continue;
                    }
                }

            } catch (std::runtime_error err) {
                std::print("error while reading message: {}", err.what());
                continue;
            }
        }
    }

    void addNewClient(std::string username, sockaddr_in &clientAddress, SOCKET &clientSocket) {
        if (isUsernameInUse(username)) {
            std::string errMessage = "Username already in use!";
            //MessageBoxA(NULL, errMessage.c_str(), "Client Creation Error", MB_OK | MB_ICONERROR);
            closesocket(clientSocket);
            throw errMessage;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::print("Accepted connection from {}:{}\n", clientIP, ntohs(clientAddress.sin_port));

        clients[username] = &clientSocket;

        // Handle client in a separate thread
        std::thread clientThread([this, &clientSocket]() { this->handleClient(clientSocket); });
        clientThread.detach();
    }


};