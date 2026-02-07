#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <stdexcept>
#include "exceptions.h"

enum MessageType {
    CONNECT,
    CONNECT_ACK,
    GROUP_MESSAGE,
    SERVER_GROUP_MESSAGE,
    PRIVATE_MESSAGE,
    USERS_LIST_UPDATE
};

struct Header {
    MessageType type;
    uint32_t length;
};


struct ConnectMessagePayload {
    std::string username;
};

struct GroupMessagePayload {
    std::string message;
};

struct ServerGroupMessagePayload {
    std::string username;
    std::string message;
};

struct UsersListUpdatePayload {
    std::vector<std::string> usernames;
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

void sendConnectACKMessage(SOCKET &socket) {
    uint32_t payloadLength = 0;
    Header header = { CONNECT_ACK, payloadLength };

    std::vector<char> buffer(sizeof(Header) + payloadLength);
    char* bufferPointer = buffer.data();

    writeu32(bufferPointer, header.type);
    writeu32(bufferPointer, header.length);

    send(socket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void sendGroupMessage(SOCKET &socket, GroupMessagePayload &payload) {
    uint32_t messageLength = payload.message.size();
    if (messageLength == 0) {
        throw "message field is required";
    }

    uint32_t payloadLength = sizeof(uint32_t) + messageLength;
    Header header = { GROUP_MESSAGE, payloadLength };

    std::vector<char> buffer(sizeof(Header) + payloadLength);
    char* bufferPointer = buffer.data();

    writeu32(bufferPointer, header.type);
    writeu32(bufferPointer, header.length);

    writeu32(bufferPointer, messageLength);
    writeString(bufferPointer, payload.message);

    send(socket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void sendServerGroupMessage(SOCKET &socket, ServerGroupMessagePayload &payload) {
    uint32_t messageLength = payload.message.size();
    uint32_t usernameLength = payload.username.size();
    if (messageLength == 0 || usernameLength == 0) {
        throw "message and username field are required";
    }

    uint32_t payloadLength = sizeof(uint32_t) + messageLength + sizeof(uint32_t) + usernameLength;
    Header header = { SERVER_GROUP_MESSAGE, payloadLength };

    std::vector<char> buffer(sizeof(Header) + payloadLength);
    char* bufferPointer = buffer.data();

    writeu32(bufferPointer, header.type);
    writeu32(bufferPointer, header.length);

    writeu32(bufferPointer, messageLength);
    writeString(bufferPointer, payload.message);

    writeu32(bufferPointer, usernameLength);
    writeString(bufferPointer, payload.username);

    send(socket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void sendUsersListUpdateMessage(SOCKET &socket, UsersListUpdatePayload &payload) {
    uint32_t usersCount = payload.usernames.size();
    uint32_t payloadLength = sizeof(uint32_t);

    for (const std::string &username : payload.usernames) {
        payloadLength += sizeof(uint32_t) + username.size();
    }

    Header header = { USERS_LIST_UPDATE, payloadLength };

    std::vector<char> buffer(sizeof(Header) + payloadLength);
    char* bufferPointer = buffer.data();

    writeu32(bufferPointer, header.type);
    writeu32(bufferPointer, header.length);

    writeu32(bufferPointer, usersCount);
    for (const std::string &username : payload.usernames) {
        writeu32(bufferPointer, username.size());
        writeString(bufferPointer, username);
    }

    send(socket, buffer.data(), static_cast<int>(buffer.size()), 0);
}

void readMessageHeader(SOCKET &socket, Header &header) {
    std::vector<char> buffer(sizeof(Header));
    receiveAll(socket, buffer.data(), buffer.size());
    
    char* bufferPointer = buffer.data();
    readu32(bufferPointer, reinterpret_cast<uint32_t&>(header.type));
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

void readGroupMessage(SOCKET &socket, Header &header, GroupMessagePayload &payload) {
    std::vector<char> payloadBuffer(header.length);
    receiveAll(socket, payloadBuffer.data(), payloadBuffer.size());

    char* bufferPointer = payloadBuffer.data();

    uint32_t messageLenght;
    readu32(bufferPointer, messageLenght);

    readString(bufferPointer, payload.message, messageLenght);
}

void readServerGroupMessage(SOCKET &socket, Header &header, ServerGroupMessagePayload &payload) {
    std::vector<char> payloadBuffer(header.length);
    receiveAll(socket, payloadBuffer.data(), payloadBuffer.size());

    char* bufferPointer = payloadBuffer.data();

    uint32_t messageLenght;
    readu32(bufferPointer, messageLenght);
    readString(bufferPointer, payload.message, messageLenght);

    uint32_t usernameLenght;
    readu32(bufferPointer, usernameLenght);
    readString(bufferPointer, payload.username, usernameLenght);
}

void readUsersListUpdateMessage(SOCKET &socket, Header &header, UsersListUpdatePayload &payload) {
    std::vector<char> payloadBuffer(header.length);
    receiveAll(socket, payloadBuffer.data(), payloadBuffer.size());

    char* bufferPointer = payloadBuffer.data();

    uint32_t usersCount;
    readu32(bufferPointer, usersCount);

    for (uint32_t i = 0; i < usersCount; i++) {
        uint32_t usernameLenght;
        readu32(bufferPointer, usernameLenght);

        std::string username;
        readString(bufferPointer, username, usernameLenght);
        payload.usernames.push_back(username);
    }
}
