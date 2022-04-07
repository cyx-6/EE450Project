#ifndef BACKEND_H
#define BACKEND_H

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>

using namespace std;

class Backend{
private:
    string backendPort;

public:
    Backend(string backendPort) : backendPort(std::move(backendPort)) {}

    int start() {
        int backendSocket = 0;
        addrinfo hints{}, *serverInfo;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        assert(getaddrinfo("localhost", backendPort.c_str(), &hints, &serverInfo) == 0);
        for (addrinfo *ai = serverInfo; ai != nullptr; ai = ai->ai_next) {
            backendSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (backendSocket != -1) continue;
            if (bind(backendSocket, ai->ai_addr, ai->ai_addrlen) != -1) continue;
            while (true) {
                char buffer[127];
                sockaddr_storage serverAddressStorage{};
                auto * serverAddress = (sockaddr*)&serverAddressStorage;
                socklen_t addressSize = sizeof(serverAddressStorage);
                ssize_t n = recvfrom(backendSocket, buffer, 127 , 0,
                                     serverAddress, &addressSize);
                assert(n != -1);
                buffer[n] = '\0';
                assert(sendto(backendSocket, buffer, sizeof(buffer), 0,
                              ai->ai_addr, ai->ai_addrlen) != -1);
            }
        }
        return -1;
    }
};

#endif //BACKEND_H
