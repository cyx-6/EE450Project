#ifndef SERVER_H
#define SERVER_H

#include <algorithm>
#include <arpa/inet.h>
#include <cassert>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <random>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "config.h"
#include "operation.h"
#include "transaction.h"
#include "user.h"
#include "utils.h"

using namespace std;

class Server {
private:
    vector<uint16_t> TCPPortList;
    uint16_t UDPPort;
    vector<uint16_t> backendPortList;
    vector<sockaddr_in> backendAddressList;
    vector<string> backendNameList;
    unordered_map<uint16_t, string> clientNameMap;
    int maxSerialID;
    bool initialized;

    static void sigchld_handler(int s) {
        int saved_errno = errno;
        while (waitpid(-1, nullptr, WNOHANG) > 0)
            ;
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
        TCPSocket = socket(serverAddress.sin_family, SOCK_STREAM, 0);
        assert(TCPSocket != -1);
        assert(setsockopt(TCPSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != -1);
        auto *socketAddress = (sockaddr *) &serverAddress;
        int r = bind(TCPSocket, socketAddress, sizeof(serverAddress));
        if (r == -1) {
            close(TCPSocket);
            assert(false);
        }
        assert(listen(TCPSocket, 8) != -1);
        struct sigaction sigAction {};
        sigAction.sa_handler = sigchld_handler;// reap all dead processes
        sigemptyset(&sigAction.sa_mask);
        sigAction.sa_flags = SA_RESTART;
        assert(sigaction(SIGCHLD, &sigAction, nullptr) != -1);
        return TCPSocket;
    }

    static int UDPSender(uint16_t serverPort) {
        int UDPSocket;
        sockaddr_in serverAddress{};
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(serverPort);
        serverAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        UDPSocket = socket(serverAddress.sin_family, SOCK_DGRAM, 0);
        assert(UDPSocket != -1);
        auto *socketAddress = (sockaddr *) &serverAddress;
        assert(bind(UDPSocket, socketAddress, sizeof(serverAddress)) != -1);
        return UDPSocket;
    }

    User getUserInfo(int UDPSocket, const Operation &o) {
        User u = User::initialUser(o.getUserName1());
        int k = 0;
        for (sockaddr_in backendAddress: backendAddressList) {
            cout << "The main server sent a request to server " + backendNameList[k] << "." << endl;
            auto *address = (sockaddr *) &backendAddress;
            socklen_t addressSize = sizeof(backendAddress);
            UDPSendObject(UDPSocket, address,
                          addressSize, o);
            u.merge(UDPReceiveObject<User>(UDPSocket));
            cout << "The main server received transactions from Server " + backendNameList[k] +
                            " using UDP over port " + to_string(backendPortList[k])
                 << "." << endl;
            ++k;
        }
        return u;
    }

    int TXList(int UDPSocket, const Operation &o) {
        vector<Transaction> transactions;
        for (sockaddr_in backendAddress: backendAddressList) {
            auto *address = (sockaddr *) &backendAddress;
            socklen_t addressSize = sizeof(backendAddress);
            UDPSendObject(UDPSocket, address,
                          addressSize, o);
            int n = UDPReceiveInt(UDPSocket);
            for (int i = 0; i < n; ++i)
                transactions.emplace_back(UDPReceiveObject<Transaction>(UDPSocket));
        }
        sort(transactions.begin(), transactions.end(), Transaction::comp);
        ofstream file(Config::TXLIST_FILE);
        file << "Serial No.\tSender\tReceiver\tTransfer Amount" << endl;
        for (const Transaction &t: transactions)
            file << t.toString() << endl;
        file.close();
        return 0;
    }

    int checkWallet(int UDPSocket, int TCPSocket, uint16_t port, const Operation &o) {
        cout << "The main server received input=" + o.getUserName1() +
                        " from the client using TCP over port " + to_string(port) + "."
             << endl;
        User u = getUserInfo(UDPSocket, o);
        if (u.exist()) u.addInitialBalance();
        TCPSendObject(TCPSocket, u);
        cout << "The main server sent the current balance to client " + clientNameMap.at(port) << "." << endl;
        return 0;
    }

    int TXCoins(int UDPSocket, int TCPSocket, uint16_t port, Operation &o, size_t k) {
        pair<Operation, Operation> p = o.toSubOperation();
        cout << "The main server received from " + o.getUserName1() + " to transfer " +
                        to_string(o.getTransferAmount()) + " coins to " +
                        o.getUserName2() + " using TCP over port " + to_string(port) + "."
             << endl;
        User u = getUserInfo(UDPSocket, p.first), v = getUserInfo(UDPSocket, p.second);
        if (u.exist() and v.exist() and u.transferable(o)) {
            auto *address = (sockaddr *) &backendAddressList[k];
            socklen_t addressSize = sizeof(backendAddressList[k]);
            cout << "The main server sent a request to server " + backendNameList[k] << "." << endl;
            o.setSerialID(maxSerialID);
            UDPSendObject(UDPSocket, address, addressSize, o);
            assert(UDPReceiveInt(UDPSocket) == maxSerialID);
            cout << "The main server received the feedback from server " + backendNameList[k] +
                            " using UDP over port " + to_string(backendPortList[k])
                 << "." << endl;
            ++maxSerialID;
        }
        if (u.exist()) u.addInitialBalance();
        if (v.exist()) v.addInitialBalance();
        TCPSendPrimitive(TCPSocket, 2);
        assert(TCPReceiveString(TCPSocket) == o.getUserName1());
        TCPSendObject(TCPSocket, u);
        assert(TCPReceiveString(TCPSocket) == o.getUserName2());
        TCPSendObject(TCPSocket, v);
        cout << "The main server sent the result of the transaction to client " << clientNameMap.at(port) << "." << endl;
        return 0;
    }

    int stats(int UDPSocket, int TCPSocket, const Operation &o) {
        unordered_map<string, User> users;
        for (sockaddr_in backendAddress: backendAddressList) {
            auto *address = (sockaddr *) &backendAddress;
            socklen_t addressSize = sizeof(backendAddress);
            UDPSendObject(UDPSocket, address,
                          addressSize, o);
            int n = UDPReceiveInt(UDPSocket, nullptr, nullptr);
            for (int i = 0; i < n; ++i) {
                User u = UDPReceiveObject<User>(UDPSocket, nullptr,
                                                nullptr);
                string userName = u.getUserName();
                if (!users.count(userName))
                    users.insert(make_pair(userName, User::initialUser(userName)));
                users.at(userName).merge(u);
            }
        }
        vector<User> v;
        v.reserve(users.size());
        for (pair<string, User> p: users) v.emplace_back(p.second);
        sort(v.begin(), v.end(), User::comp);
        unsigned long n = v.size();
        TCPSendPrimitive(TCPSocket, n);
        for (int i = 0; i < n; ++i) {
            v[i].setRanking(i + 1);
            assert(TCPReceiveInt(TCPSocket) == i + 1);
            TCPSendObject(TCPSocket, v[i]);
        }
        return 0;
    }

    int UDPInitialization(int UDPSocket) {
        backendNameList.reserve(backendAddressList.size());
        for (const sockaddr_in &backendAddress: backendAddressList) {
            auto *address = (sockaddr *) &backendAddress;
            Operation o;
            UDPSendObject(UDPSocket, address,
                          sizeof(backendAddress), o);
            backendNameList.emplace_back(UDPReceiveString(UDPSocket));
            maxSerialID = max(maxSerialID, UDPReceiveInt(UDPSocket));
        }
        return 0;
    }

public:
    explicit Server(const vector<uint16_t> &TCPPortList,
                    uint16_t UDPPort,
                    const vector<uint16_t> &backendPortList) : TCPPortList(TCPPortList),
                                                               UDPPort(UDPPort),
                                                               backendPortList(backendPortList),
                                                               maxSerialID(-1) {
        backendAddressList.reserve(backendPortList.size());
        for (uint16_t port: backendPortList) {
            sockaddr_in backendAddress{};
            memset(&backendAddress, 0, sizeof(backendAddress));
            backendAddress.sin_family = AF_INET;
            backendAddress.sin_port = htons(port);
            backendAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
            backendAddressList.emplace_back(backendAddress);
        }
        initialized = false;
    }

    void start() {
        pollfd polls[TCPPortList.size()];
        int UDPSocket = UDPSender(UDPPort);
        int TCPPortCount = 0;
        for (uint16_t port: TCPPortList) {
            polls[TCPPortCount].fd = TCPListener(port);
            polls[TCPPortCount].events = POLLIN;
            ++TCPPortCount;
            clientNameMap.insert(make_pair(port, ""));
        }
        cout << "The main server is up and running." << endl;
        random_device device;
        mt19937 generator(device());
        uniform_int_distribution<size_t> distribution(0, backendPortList.size() - 1);
        while (true) {
            assert(poll(polls, TCPPortCount, -1) != -1);
            for (int i = 0; i < TCPPortCount; ++i) {
                if (polls[i].revents & POLLIN) {
                    sockaddr_storage clientAddressStorage{};
                    auto *clientAddress = (sockaddr *) &clientAddressStorage;
                    socklen_t clientAddressSize = sizeof(clientAddressStorage);
                    int TCPSocket = accept(polls[i].fd, clientAddress, &clientAddressSize);
                    assert(TCPSocket != -1);
                    clientNameMap.at(TCPPortList[i]) = TCPReceiveString(TCPSocket);
                    TCPSendPrimitive(TCPSocket, clientNameMap.at(TCPPortList[i]));
                    Operation o = TCPReceiveObject<Operation>(TCPSocket);
                    if (!initialized) {
                        UDPInitialization(UDPSocket);
                        initialized = true;
                    }
                    switch (o.getType()) {
                        case Operation::Type::CHECK_WALLET:
                            checkWallet(UDPSocket, TCPSocket, TCPPortList[i], o);
                            break;
                        case Operation::Type::TXCOINS:
                            TXCoins(UDPSocket, TCPSocket, TCPPortList[i], o, distribution(generator));
                            break;
                        case Operation::Type::TXLIST:
                            TXList(UDPSocket, o);
                            break;
                        case Operation::Type::STATS:
                            stats(UDPSocket, TCPSocket, o);
                            break;
                        default:
                            assert(false);
                    }
                    close(TCPSocket);
                }
            }
        }
    }
};

#endif//SERVER_H
