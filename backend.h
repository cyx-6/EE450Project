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

#include "transaction.h"

using namespace std;

class Backend{
private:
    string backendName;
    uint16_t backendPort;

public:
    Backend(string backendName,
            uint16_t backendPort) :
            backendName(std::move(backendName)),
            backendPort(backendPort) {}

    int start() const {
        int backendSocket = 0;
        sockaddr_in backendAddress{};
        memset(&backendAddress, 0, sizeof(backendAddress));
        backendAddress.sin_family = AF_INET;
        backendAddress.sin_port = htons(backendPort);
        backendAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        backendSocket = socket(backendAddress.sin_family, SOCK_DGRAM, 0);
        assert(backendSocket != -1);
        auto* socketAddress = (sockaddr*) &backendAddress;
        assert(bind(backendSocket, socketAddress, sizeof(backendAddress)) != -1);
        cout << "The Server" + backendName + " is up and running using UDP on port " + to_string(backendPort) << "." << endl;
        while (true) {
            char buffer[127];
            sockaddr_storage serverAddressStorage{};
            auto * serverAddress = (sockaddr*)&serverAddressStorage;
            socklen_t addressSize = sizeof(serverAddressStorage);
            ssize_t n = recvfrom(backendSocket, buffer, 127 , 0,
                                 serverAddress, &addressSize);
            assert(n != -1);
            cout << "The Server" + backendName + " received a request from the Main Server." << endl;
            strcat(buffer, "backend");
            assert(sendto(backendSocket, buffer, sizeof(buffer), 0,
                          serverAddress, addressSize) != -1);
            cout << "The Server" + backendName +  " finished sending the response to the Main Server." << endl;
        }
        return 0;
    }
};

#endif //BACKEND_H
