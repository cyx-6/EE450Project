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
#include <netinet/in.h>
//#include <utility>

using namespace std;

class Client {
private:
    string clientName, userName;
    uint16_t serverPort;

    int connectToServer() {
        int clientSocket;
        sockaddr_in serverAddress{};
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
        clientSocket = socket(serverAddress.sin_family,SOCK_STREAM,0);
        assert(clientSocket != -1);
        auto* socketAddress = (sockaddr*) &serverAddress;;
        int r = connect(clientSocket, socketAddress, sizeof(serverAddress));
        if (r == -1) {
            close(clientSocket);
            assert(false);
        }
        char buffer[127] = "test";
        assert(send(clientSocket, buffer, sizeof(buffer), 0) != -1);
        assert(recv(clientSocket, buffer, 127, 0) != -1);
        cout << "client" + clientName + ": " + string(buffer) << endl;
        close(clientSocket);
        return 0;
    }

public:
    Client(string clientName, string userName, uint16_t serverPort) :
            clientName(std::move(clientName)),
            userName(std::move(userName)),
            serverPort(serverPort) {
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
