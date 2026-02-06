#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <print>
#include <format>
#include <map>
#include <functional>

#include "message.h"
#include "exceptions.h"

using ActionMapT = std::map<MessageType, std::function<void(SOCKET &socket, Header &header)>>;

class Client {
private:
    std::string host;
    int port;
    SOCKET clientSocket;
    sockaddr_in serverAddress = {};
    ActionMapT actionMap;

public:
    Client(ActionMapT _actionMap = {}, std::string _host = "127.0.0.1", int _port = 8000): host(_host), port(_port), actionMap(_actionMap) {}

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

    void runFromMap(Header &header) {
        auto it = actionMap.find(header.type);
        if (it != actionMap.end()) {
            it->second(clientSocket, header);
        }
        return;
    }

    void listenForServerMessages() {
        while(true) {
            try {
                Header header;
                try {
                    readMessageHeader(clientSocket, header);
                } catch (const ConnectionClosedException &err) {
                    throw FatalClientException("Connection closed by the server, killing thread...\n");
                } catch (const MessageReceiveException &err) {
                    throw FatalClientException("Message receive error, killing thread...\n");
                }

                runFromMap(header);

            } catch (const FatalServerException &err) {
                std::print("fatal server exception: {}\n", err.what());
                return;
            }
        }
    }

    void connectToServer(std::string username) {
        if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
            std::string errMessage = "Connection failed with error: " + WSAGetLastError();
            closesocket(clientSocket);
            throw FatalServerException(errMessage);
        }

        ConnectMessagePayload payload = { username };
        sendConnectMessage(clientSocket, payload);

        std::thread listenToServer([this]() { this->listenForServerMessages(); });
        listenToServer.detach();
    }

    void sendMessage(std::string message) {
        GroupMessagePayload payload = { message };
        sendGroupMessage(clientSocket, payload);
    }
};