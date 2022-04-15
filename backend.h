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
    uint16_t backendPort, serverPort;
    unordered_map<string, User> users;
    vector<Transaction> transactions;

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
        pair<User, User> p = User::fromTransaction(t);
        if (!users.count(userName1))
            users.insert(make_pair(userName1, p.first));
        else
            users.at(userName1).merge(p.first);
        if (!users.count(userName2))
            users.insert(make_pair(userName2, p.second));
        else
            users.at(userName2).merge(p.second);
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
        for (const User &u: v)
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
            pair<User, User> p = User::fromTransaction(t);
            if (!users.count(userName1))
                users.insert(make_pair(userName1, p.first));
            else
                users.at(userName1).merge(p.first);
            if (!users.count(userName2))
                users.insert(make_pair(userName2, p.second));
            else
                users.at(userName2).merge(p.second);
            ++n;
        }
        sort(transactions.begin(), transactions.end(), Transaction::comp);
        file.close();
        return 0;
    }

public:
    Backend(string backendName, string fileName,
            uint16_t backendPort, uint16_t serverPort) : backendName(std::move(backendName)),
                                                         fileName(std::move(fileName)),
                                                         backendPort(backendPort),
                                                         serverPort(serverPort) {}

    int start() {
        int backendSocket;
        load();
        sockaddr_in backendAddressIn = socketAddress(backendPort);
        backendSocket = socket(backendAddressIn.sin_family, SOCK_DGRAM, 0);
        assert(backendSocket != -1);
        assert(bind(backendSocket, (sockaddr *) &backendAddressIn, sizeof(backendAddressIn)) != -1);
        sockaddr_in serverAddressIn = socketAddress(serverPort);
        auto *serverAddress = (sockaddr *) &serverAddressIn;
        unsigned long serverAddressSize = sizeof(serverAddressIn);
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, backendName);
        UDPSendPrimitive(backendSocket, serverAddress,
                         serverAddressSize, transactions.back().getSerialID());
        assert(UDPReceiveInt(backendSocket) >= transactions.back().getSerialID());
        cout << "The Server" + backendName + " is up and running using UDP on port " + to_string(backendPort) << "." << endl;
        while (true) {
            Operation o = UDPReceiveObject<Operation>(backendSocket);
            cout << "The Server" + backendName + " received a request from the Main Server." << endl;
            switch (o.getType()) {
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
            cout << "The Server" + backendName + " finished sending the response to the Main Server." << endl;
        }
        return 0;
    }
};

#endif//BACKEND_H
