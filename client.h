#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
//#include <utility>

#include "config.h"
#include "operation.h"
#include "utils.h"

using namespace std;

class Client {
private:
    string clientName;
    uint16_t serverPort;


public:
    Client(string clientName, uint16_t serverPort) : clientName(std::move(clientName)),
                                                     serverPort(serverPort) {
    }

    void checkWallet(){};
    void TXCoins(){};
    void TXList(){};
    void stats(){};
    int run(int argc, char **argv) {
        int clientSocket;
        sockaddr_in serverAddress{};
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        serverAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        clientSocket = socket(serverAddress.sin_family, SOCK_STREAM, 0);
        assert(clientSocket != -1);
        auto *socketAddress = (sockaddr *) &serverAddress;

        int r = connect(clientSocket, socketAddress, sizeof(serverAddress));
        cout << "The client " + clientName + " is up and running." << endl;
        if (r == -1) {
            close(clientSocket);
            assert(false);
        }
        Operation o(argc, argv);
        TCPSendOperation(clientSocket, o);
        switch (o.getType()) {
            case Operation::Type::CHECK_WALLET:
                break;
            case Operation::Type::TXCOINS:
                break;
            case Operation::Type::TXLIST:
                break;
            case Operation::Type::STATS:
                break;
            default:
                assert(false);
        }
        close(clientSocket);
        return 0;
    }
};

#endif//CLIENT_H
