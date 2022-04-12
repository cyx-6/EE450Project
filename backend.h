#ifndef BACKEND_H
#define BACKEND_H

#include <algorithm>
#include <arpa/inet.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
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

class Backend {
private:
    string backendName, fileName;
    uint16_t backendPort;
    unordered_map<string, User> users;
    vector<Transaction> transactions;

    int backendInfo(int backendSocket, sockaddr *serverAddress, socklen_t serverAddressSize) {
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, backendName);
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, transactions.back().getSerialID());
        return 0;
    }

    int TXList(int backendSocket, sockaddr *serverAddress, socklen_t serverAddressSize) {
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, transactions.size());
        for (const Transaction &t: transactions) {
            UDPSendObject(backendSocket, serverAddress,
                          serverAddressSize, t);
        }
        return 0;
    }

    int checkWallet(int backendSocket, sockaddr *serverAddress, socklen_t serverAddressSize, const Operation &o) {
        string userName1 = o.getUserName1();
        User u = User::initialUser(o.getUserName1());
        if (users.count(userName1)) u.merge(users.at(userName1));
        UDPSendObject(backendSocket, serverAddress,
                      serverAddressSize, u);
        return 0;
    }

    int TXCoins(int backendSocket, sockaddr *serverAddress, socklen_t serverAddressSize, const Operation &o) {
        string userName1 = o.getUserName1(), userName2 = o.getUserName2();
        Transaction t(o.toTransaction());
        transactions.emplace_back(t);
        if (!users.count(userName1))
            users.insert(make_pair(userName1, User::initialUser(userName1)));
        if (!users.count(userName2))
            users.insert(make_pair(userName2, User::initialUser(userName2)));
        users.at(userName1).transfer(users.at(userName2), o);
        UDPSendPrimitive(backendSocket, serverAddress, serverAddressSize, t.getSerialID());
        return 0;
    }

    int stats(int backendSocket, sockaddr *serverAddress, socklen_t serverAddressSize) {
        vector<User> v;
        v.reserve(users.size());
        for (pair<string, User> p: users) v.emplace_back(p.second);
        sort(v.begin(), v.end(), User::comp);
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, v.size());
        for (const User &u : v)
            UDPSendObject(backendSocket, serverAddress,
                          serverAddressSize, u);
        return 0;
    }

    int load() {
        ifstream file(fileName);
        int n = 0;
        while (file.peek() != EOF) {
            string s;
            getline(file, s);
            replace(s.begin(), s.end(), ' ', char(Config::SEPARATOR));
            if (s.empty()) break;
            s.push_back(Config::SEPARATOR);
            Transaction t(s);
            transactions.emplace_back(t);
            string userName1 = t.getUserName1(), userName2 = t.getUserName2();
            int transferAmount = t.getTransferAmount();
            if (!users.count(userName1))
                users.insert(make_pair(userName1, User::initialUser(userName1)));
            users.at(userName1).merge(User::initialUser(userName1, 1, -transferAmount));
            if (!users.count(userName2))
                users.insert(make_pair(userName2, User::initialUser(userName2)));
            users.at(userName2).merge(User::initialUser(userName2, 1, transferAmount));
            ++n;
        }
        sort(transactions.begin(), transactions.end(), Transaction::comp);
        file.close();
        return 0;
    }

public:
    Backend(string backendName, string fileName, uint16_t backendPort) : backendName(std::move(backendName)), fileName(std::move(fileName)),
                                                                         backendPort(backendPort) {}

    int start() {
        int backendSocket;
        load();
        sockaddr_in backendAddress{};
        memset(&backendAddress, 0, sizeof(backendAddress));
        backendAddress.sin_family = AF_INET;
        backendAddress.sin_port = htons(backendPort);
        backendAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        backendSocket = socket(backendAddress.sin_family, SOCK_DGRAM, 0);
        assert(backendSocket != -1);
        auto *socketAddress = (sockaddr *) &backendAddress;
        assert(bind(backendSocket, socketAddress, sizeof(backendAddress)) != -1);
        cout << "The Server" + backendName + " is up and running using UDP on port " + to_string(backendPort) << "." << endl;
        while (true) {
            sockaddr_storage serverAddressStorage{};
            auto *serverAddress = (sockaddr *) &serverAddressStorage;
            socklen_t serverAddressSize = sizeof(serverAddressStorage);
            Operation o = UDPReceiveObject<Operation>(backendSocket, serverAddress,
                                                      &serverAddressSize);
            cout << "The Server" + backendName + " received a request from the Main Server." << endl;
            switch (o.getType()) {
                case Operation::Type::NONE:
                    backendInfo(backendSocket, serverAddress, serverAddressSize);
                    break;
                case Operation::Type::CHECK_WALLET:
                    checkWallet(backendSocket, serverAddress, serverAddressSize, o);
                    break;
                case Operation::Type::TXCOINS:
                    TXCoins(backendSocket, serverAddress, serverAddressSize, o);
                    break;
                case Operation::Type::TXLIST:
                    TXList(backendSocket, serverAddress, serverAddressSize);
                    break;
                case Operation::Type::STATS:
                    stats(backendSocket, serverAddress, serverAddressSize);
                    break;
                default:
                    assert(false);
            }
            if (o.getType() != Operation::Type::TXCOINS)
                cout << "The Server" + backendName + " finished sending the response to the Main Server." << endl;
        }
        return 0;
    }
};

#endif//BACKEND_H
