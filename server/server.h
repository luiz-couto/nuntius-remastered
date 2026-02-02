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
#include "exceptions.h"

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
    Server(int _port = 8000): port(_port) {}

    void startAndListen() {
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
        while(true) {
            sockaddr_in clientAddress = {};
            int clientAddressLen = sizeof(clientAddress);
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientAddressLen);
            if (clientSocket == INVALID_SOCKET) {
                std::string errMessage = "Accept client failed with error: " + WSAGetLastError();
                std::print("{}\n", errMessage);
                continue;
            }

            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::print("Accepted socket connection from {}:{}\n", clientIP, ntohs(clientAddress.sin_port));

            // Handle client in a separate thread
            std::thread clientThread([this, &clientSocket]() { this->handleClient(clientSocket); });
            clientThread.detach();
        }
    }

    bool isUsernameInUse(std::string username) {
        auto it = clients.find(username);
        return it != clients.end();
    }

    void handleClient(SOCKET &socket) {
        std::string username = "";
        while(true) {
            try {
                Header header;
                try {
                    readMessageHeader(socket, header);
                } catch (const ConnectionClosedException &err) {
                    throw FatalClientException("Connection closed by the client, killing thread...\n");
                } catch (const MessageReceiveException &err) {
                    throw FatalClientException("Message receive error, killing thread...\n");
                }

                switch(header.type) {
                    case CONNECT: {
                        ConnectMessagePayload payload;
                        try {
                            readConnectMessage(socket, header, payload);
                        } catch (std::runtime_error err) {
                            throw FatalClientException("Failed to connect with client, closing socket connection...");
                        }
                        std::print("Received connection request with username: {}\n", payload.username);
                        this->addNewClient(payload.username, socket);
                        username = payload.username;
                        break;
                    }
                    default: {
                        continue;
                    }
                }

            } catch (const FatalClientException &err) {
                std::print("fatal client exception: {}\n", err.what());
                this->removeClient(username, socket);
                return;
            }
        }
    }

    void addNewClient(std::string username, SOCKET &clientSocket) {
        if (isUsernameInUse(username)) {
            std::string errMessage = "Username already in use!";
            //MessageBoxA(NULL, errMessage.c_str(), "Client Creation Error", MB_OK | MB_ICONERROR);
            closesocket(clientSocket);
            throw errMessage;
        }

        clients[username] = &clientSocket;
        std::print("client {} added!\n", username);
    }

    void removeClient(std::string username, SOCKET &socket) {
        this->disconnectClient(socket);

        auto it = clients.find(username);
        if (it != clients.end()) {
            clients.erase(it);
            std::print("client {} removed\n", username);
        }
    }

    void disconnectClient(SOCKET &socket) {
        closesocket(socket);
    }

};