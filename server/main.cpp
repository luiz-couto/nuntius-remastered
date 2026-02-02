#include "server.h"

int main() {
    Server *server = new Server();
    server->startAndListen();
    return 0;
}