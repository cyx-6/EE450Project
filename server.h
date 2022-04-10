#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <csignal>
#include <vector>
#include <utility>
#include <poll.h>
#include <netinet/in.h>
//#include <utility>

#include "operation.h"
#include "transaction.h"
#include "user.h"
#include "config.h"

using namespace std;

class Server {
private:
    vector<uint16_t> TCPPortList;
    uint16_t UDPPort;
    vector<uint16_t> backendPortList;
    vector<sockaddr_in> backendAddressList;
    long int maxSerialID{};

    static void sigchld_handler(int s)
    {
        int saved_errno = errno;
        while(waitpid(-1, nullptr, WNOHANG) > 0);
        errno = saved_errno;
    }

    static int TCPListener(uint16_t serverPort) {
        int TCPSocket;
        sockaddr_in serverAddress{};
        memset(&serverAddress, 0, sizeof(serverAddress));
        int yes = 1;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        serverAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        TCPSocket = socket(serverAddress.sin_family,SOCK_STREAM,0);
        assert(TCPSocket != -1);
        assert(setsockopt(TCPSocket, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) != -1);
        auto* socketAddress = (sockaddr*) &serverAddress;
        int r = bind(TCPSocket, socketAddress, sizeof(serverAddress));
        if (r == -1) {
            close(TCPSocket);
            assert(false);
        }
        assert(listen(TCPSocket, 8) != -1);
        struct sigaction sigAction{};
        sigAction.sa_handler = sigchld_handler; // reap all dead processes
        sigemptyset(&sigAction.sa_mask);
        sigAction.sa_flags = SA_RESTART;
        assert(sigaction(SIGCHLD, &sigAction, nullptr) != -1);
        return TCPSocket;
    }

    static int UDPSender(uint16_t serverPort) {
        int UDPSocket = 0;
        sockaddr_in serverAddress{};
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        serverAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        UDPSocket = socket(serverAddress.sin_family, SOCK_DGRAM, 0);
        assert(UDPSocket != -1);
        auto* socketAddress = (sockaddr*) &serverAddress;
        assert(bind(UDPSocket, socketAddress, sizeof(serverAddress)) != -1);
        return UDPSocket;
    }

    int getUserInfo(int UDPSocket, const string& userName) {
        for (sockaddr_in backendAddress : backendAddressList) {
            char buffer[Config::BUFFER_LEN];
            auto * address = (sockaddr*)&backendAddress;
            ssize_t n = sendto(UDPSocket, buffer, Config::BUFFER_SIZE, 0,
                       address, sizeof(backendAddress));
            assert(n != -1);
            sockaddr_storage addressStorage{};
            auto * serverAddress = (sockaddr*)&addressStorage;
            socklen_t addressSize = sizeof(addressStorage);
            n = recvfrom(UDPSocket, buffer, Config::BUFFER_SIZE , 0,
                         serverAddress, &addressSize);
            strcat(buffer, "server");
        }
    }

public:
    explicit Server(const vector<uint16_t>& TCPPortList,
                    uint16_t UDPPort,
                    const vector<uint16_t>& backendPortList) :
                    TCPPortList(TCPPortList),
                    UDPPort(UDPPort),
                    backendPortList(backendPortList) {
        for (uint16_t port : backendPortList) {
            sockaddr_in backendAddress{};
            memset(&backendAddress, 0, sizeof(backendAddress));
            backendAddress.sin_family = AF_INET;
            backendAddress.sin_port = htons(port);
            backendAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
            backendAddressList.emplace_back(backendAddress);
        }
    }

    void start() {
        pollfd polls[TCPPortList.size()];
        int UDPSocket = UDPSender(UDPPort);
        int TCPPortCount = 0;
        for (uint16_t port : TCPPortList) {
            polls[TCPPortCount].fd = TCPListener(port);
            polls[TCPPortCount].events = POLLIN;
            ++TCPPortCount;
        }
        for (const sockaddr_in& backendAddress : backendAddressList) {
            auto * address = (sockaddr*)&backendAddress;
            Operation o;
            char buffer[Config::BUFFER_LEN];
            o.encode(buffer);
            assert(sendto(UDPSocket, buffer, Config::BUFFER_SIZE, 0,
                          address, sizeof(backendAddress)) != -1);
            assert(recvfrom(UDPSocket, buffer, Config::BUFFER_SIZE , 0,
                         nullptr, nullptr) != -1);
            maxSerialID = max(maxSerialID, strtol(buffer, nullptr, 10));
        }
        cout << maxSerialID << endl;
        cout << "The main server is up and running." << endl;
        while (true) {
            assert(poll(polls, TCPPortCount, -1) != -1);
            for (int i = 0; i < TCPPortCount; ++i) {
                if (polls[i].revents & POLLIN) {
                    sockaddr_storage clientAddressStorage{};
                    auto * clientAddress = (sockaddr*)&clientAddressStorage;
                    socklen_t clientAddressSize = sizeof(clientAddressStorage);
                    int TCPSocket = accept(polls[i].fd, clientAddress, &clientAddressSize);
                    assert(TCPSocket != -1);
                    char buffer[Config::BUFFER_LEN];
                    ssize_t n = recv(TCPSocket, buffer, Config::BUFFER_SIZE, 0);
                    Operation o(buffer);
//                    operation.print();
                    switch (o.getType()) {
                        case Operation::Type::CHECK_WALLET:
//                            checkWallet(backendSocket, serverAddress, serverAddressSize, o);
                            break;
                        case Operation::Type::TXCOINS:
//                            TXCoins(o);
                            break;
                        case Operation::Type::TXLIST:
//                            TXList(backendSocket, serverAddress, serverAddressSize);
                            break;
                        case Operation::Type::STATS:
//                            stats(backendSocket, serverAddress, serverAddressSize);
                            break;
                        default:
                            assert(false);
                    }
                    strcat(buffer, "server");
                    
                    assert(n != -1);
                    assert(send(TCPSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
                    close(TCPSocket);
                }
            }
        }
    }
};

#endif //SERVER_H
