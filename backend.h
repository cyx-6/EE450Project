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
#include <unordered_map>
#include <vector>
#include <fstream>
#include <algorithm>

#include "transaction.h"
#include "operation.h"
#include "user.h"
#include "config.h"

using namespace std;

class Backend{
private:
    string backendName, fileName;
    uint16_t backendPort;
    unordered_map<string, User> users;
    vector<Transaction> transactions;

    int maxSerialID(int backendSocket, sockaddr* serverAddress, socklen_t serverAddressSize) {
        string s = to_string(transactions.back().getSerialID());
        assert(sendto(backendSocket, s.c_str(), sizeof(s.c_str()), 0,
                      serverAddress, serverAddressSize) != -1);
        return 0;
    }

    int TXList(int backendSocket, sockaddr* serverAddress, socklen_t serverAddressSize) {
        string s = to_string(transactions.size());
        assert(sendto(backendSocket, s.c_str(), sizeof(s.c_str()), 0,
                      serverAddress, serverAddressSize) != -1);
        for (const Transaction& t: transactions) {
            char buffer[Config::BUFFER_LEN];
            t.encode(buffer);
            assert(sendto(backendSocket, buffer, Config::BUFFER_SIZE, 0,
                          serverAddress, serverAddressSize) != -1);
        }
        return 0;
    }

    int checkWallet(int backendSocket, sockaddr* serverAddress, socklen_t serverAddressSize, const Operation& o) {
        char buffer[Config::BUFFER_LEN];
        string userName1 = o.getUserName1();
        User u(0, userName1, 0, 0);
        if (users.count(userName1)) u.merge(users.at(userName1));
        u.encode(buffer);
        assert(sendto(backendSocket, buffer, Config::BUFFER_SIZE, 0,
                      serverAddress, serverAddressSize) != -1);
        return 0;
    }

    int TXCoins(const Operation& o) {
        transactions.emplace_back(o.toTransaction());
        return 0;
    }

    int stats(int backendSocket, sockaddr* serverAddress, socklen_t serverAddressSize) {
        vector<User> v;
        for (pair<string, User> p : users) v.emplace_back(p.second);
        sort(v.begin(), v.end(), User::comp);
        unsigned long n = v.size();
        string s = to_string(n);
        assert(sendto(backendSocket, s.c_str(), sizeof(s.c_str()), 0,
                      serverAddress, serverAddressSize) != -1);
        for (int i = 0; i < n; ++i) {
            char buffer[Config::BUFFER_LEN];
            v[i].setRanking(i + 1);
            v[i].encode(buffer);
            assert(sendto(backendSocket, buffer, Config::BUFFER_SIZE, 0,
                          serverAddress, serverAddressSize) != -1);
        }
        return 0;
    }

    long int load() {
        ifstream file(fileName);
        int n = 0;
        while (file.peek() != EOF) {
            string s;
            getline(file, s);
            replace(s.begin(), s.end(), ' ', '\t');
            if (s.empty()) break;
            Transaction t(s.c_str());
            transactions.emplace_back(t);
            string userName1 = t.getUserName1(), userName2 = t.getUserName2();
            long int transferAmount = t.getTransferAmount();
            if (!users.count(userName1))
                users.insert(make_pair(userName1, User(0, userName1, 0, 0)));
            users.at(userName1).merge(User(0, userName1, 1, -transferAmount));
            if (!users.count(userName2))
                users.insert(make_pair(userName2, User(0, userName2, 0, 0)));
            users.at(userName2).merge(User(0, userName2, 1, transferAmount));
            ++n;
        }
        sort(transactions.begin(), transactions.end(), Transaction::comp);
        file.close();
        return 0;
    }

public:
    Backend(string backendName, string fileName, uint16_t backendPort) :
    backendName(std::move(backendName)), fileName(std::move(fileName)),
    backendPort(backendPort) {}

    int start() {
        int backendSocket = 0;
        load();
        sockaddr_in backendAddress{};
        memset(&backendAddress, 0, sizeof(backendAddress));
        backendAddress.sin_family = AF_INET;
        backendAddress.sin_port = htons(backendPort);
        backendAddress.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
        backendSocket = socket(backendAddress.sin_family, SOCK_DGRAM, 0);
        assert(backendSocket != -1);
        auto* socketAddress = (sockaddr*) &backendAddress;
        assert(bind(backendSocket, socketAddress, sizeof(backendAddress)) != -1);
        cout << "The Server" + backendName + " is up and running using UDP on port " + to_string(backendPort) << "." << endl;
        while (true) {
            char buffer[Config::BUFFER_LEN];
            sockaddr_storage serverAddressStorage{};
            auto * serverAddress = (sockaddr*)&serverAddressStorage;
            socklen_t serverAddressSize = sizeof(serverAddressStorage);
            assert(recvfrom(backendSocket, buffer, Config::BUFFER_SIZE, 0,
                            serverAddress, &serverAddressSize) != -1);
            Operation o(buffer);
            if (o.getType() != Operation::Type::NONE)
                cout << "The Server" + backendName + " received a request from the Main Server." << endl;
            switch (o.getType()) {
                case Operation::Type::NONE:
                    maxSerialID(backendSocket, serverAddress, serverAddressSize);
                    break;
                case Operation::Type::CHECK_WALLET:
                    checkWallet(backendSocket, serverAddress, serverAddressSize, o);
                    break;
                case Operation::Type::TXCOINS:
                    TXCoins(o);
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
            if (o.getType() != Operation::Type::NONE)
                cout << "The Server" + backendName +  " finished sending the response to the Main Server." << endl;
        }
        return 0;
    }
};

#endif //BACKEND_H
