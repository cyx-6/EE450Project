#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>
//#include <utility>

using namespace std;

class Client {
private:
    string clientName, userName, serverPort;

    int connectToServer() {
        int clientSocket;
        addrinfo hints{}, *serverInfo;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        int rv = getaddrinfo("localhost", serverPort.c_str(), &hints, &serverInfo);
        assert(rv == 0);
        for (addrinfo *ai = serverInfo; ai != nullptr; ai = ai->ai_next) {
            clientSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (clientSocket == -1) continue;
            int r = connect(clientSocket, ai->ai_addr, ai->ai_addrlen);
            if (r == -1) {
                close(clientSocket);
                continue;
            }
            char buffer[127] = "test";
            assert(send(clientSocket, buffer, sizeof(buffer), 0) != -1);
            assert(recv(clientSocket, buffer, 127, 0) != -1);
            cout << "client" + clientName + ": " + string(buffer) << endl;
            close(clientSocket);
            return 0;
        }
        return -1;
    }

public:
    Client(string clientName, string userName, string serverPort) :
            clientName(std::move(clientName)),
            userName(std::move(userName)),
            serverPort(std::move(serverPort)) {
    }

    void checkWallet() {};
    void TXCoins() {};
    void TXList() {};
    void stats() {};
    void test() {
        connectToServer();
    }
};

#endif //CLIENT_H
