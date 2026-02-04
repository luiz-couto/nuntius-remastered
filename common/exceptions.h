#pragma once

#include <stdexcept>

class ConnectionClosedException : public std::runtime_error {
public:
    ConnectionClosedException() : std::runtime_error("Connection closed") {}
    explicit ConnectionClosedException(const std::string& msg) : std::runtime_error(msg) {}
};

class MessageReceiveException : public std::runtime_error {
public:
    MessageReceiveException() : std::runtime_error("Unable to receive message") {}
    explicit MessageReceiveException(const std::string& msg) : std::runtime_error(msg) {}
};

class FatalClientException : public std::runtime_error {
public:
    FatalClientException() : std::runtime_error("Fatal client error") {}
    explicit FatalClientException(const std::string& msg) : std::runtime_error(msg) {}
};

class FatalServerException : public std::runtime_error {
public:
    FatalServerException() : std::runtime_error("Fatal server error") {}
    explicit FatalServerException(const std::string& msg) : std::runtime_error(msg) {}
};