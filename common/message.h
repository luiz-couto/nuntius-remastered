#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "exceptions.h"

struct Header {
    uint32_t type;
    uint32_t length;
};

enum MessageType {
    CONNECT,
    GROUP_MESSAGE,
    PRIVATE_MESSAGE
};

struct ConnectMessagePayload {
    std::string username;
};

void writeu32(char* &bufferPointer, uint32_t value) {
    uint32_t outVal = htonl(value);
    memcpy(bufferPointer, &outVal, sizeof(outVal));
    bufferPointer += sizeof(outVal);
}

void writeBytes(char* &bufferPointer, const void* data, size_t len) {
    memcpy(bufferPointer, data, len);
    bufferPointer += len;
}

void writeString(char* &bufferPointer, std::string value) {
    return writeBytes(bufferPointer, value.data(), value.size());
}

void receiveAll(SOCKET &socket, void* buffer, size_t expectedLength) {
    char* bufferPointer = static_cast<char*>(buffer);
    size_t received = 0;

    while (received < expectedLength) {
        int pkgSize = recv(socket, bufferPointer + received, expectedLength - received, 0);

        if (pkgSize == 0) {
            throw ConnectionClosedException();
        }
        if (pkgSize < 0) {
            throw MessageReceiveException();
        }

        received += pkgSize;
    }
}

void readu32(char*& bufferPointer, uint32_t &value) {
    memcpy(&value, bufferPointer, sizeof(uint32_t));
    bufferPointer += sizeof(uint32_t);
    value = ntohl(value);
}

void readString(char*& bufferPointer, std::string &value, uint32_t lenght) {
    value.assign(bufferPointer, bufferPointer + lenght);
    bufferPointer += lenght;
}

void sendConnectMessage(SOCKET &socket, ConnectMessagePayload &payload) {
    uint32_t usernameLength = payload.username.size();
    if (usernameLength == 0) {
        throw "username field is required";
    }

    uint32_t payloadLength = sizeof(uint32_t) + usernameLength;
    Header header = { CONNECT, payloadLength };

    std::vector<char> buffer(sizeof(Header) + payloadLength);
    char* bufferPointer = buffer.data();

    writeu32(bufferPointer, header.type);
    writeu32(bufferPointer, header.length);

    writeu32(bufferPointer, usernameLength);
    writeString(bufferPointer, payload.username);

    send(socket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void readMessageHeader(SOCKET &socket, Header &header) {
    std::vector<char> buffer(sizeof(Header));
    receiveAll(socket, buffer.data(), buffer.size());
    
    char* bufferPointer = buffer.data();
    readu32(bufferPointer, header.type);
    readu32(bufferPointer, header.length);
}

void readConnectMessage(SOCKET &socket, Header &header, ConnectMessagePayload &payload) {
    std::vector<char> payloadBuffer(header.length);
    receiveAll(socket, payloadBuffer.data(), payloadBuffer.size());

    char* bufferPointer = payloadBuffer.data();

    uint32_t usernameLenght;
    readu32(bufferPointer, usernameLenght);

    readString(bufferPointer, payload.username, usernameLenght);
}
