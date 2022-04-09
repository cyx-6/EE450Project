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

    int TXList() {
        for (const Transaction& t: transactions) {
            char buffer[Config::BUFFER_LEN];
            t.encode(buffer);
        }
        return 0;
    }

    int checkWallet(const Operation& o) {
        char buffer[Config::BUFFER_LEN];
        string userName1 = o.getUserName1();
        User u(0, userName1, 0, 0);
        if (users.count(userName1)) u.merge(users.at(userName1));
        u.setFirst();
        u.encode(buffer);
        return 0;
    }

    int TXCoins(const Transaction& t) {
        unsigned long n = transactions.size();
        transactions.emplace_back(t);
        transactions[n].linkNext(transactions[n + 1]);
        return 0;
    }

    int stats() {
        vector<User> v;
        for (pair<string, User> p : users) v.emplace_back(p.second);
        sort(v.begin(), v.end(), User::comp);
        unsigned long n = v.size();
        v[0].setFirst();
        for (int i = 0; i < n - 1; ++i) v[i].linkNext(v[i + 1]);
        for (int i = 0; i < n; ++i) {
            char buffer[Config::BUFFER_LEN];
            v[i].encode(buffer);
            cout << buffer << endl;
        }
        return 0;
    }

    int load() {
        ifstream file(fileName);
        int n = 0;
        while (file.peek() != EOF) {
            string s;
            getline(file, s);
            replace(s.begin(), s.end(), ' ', '\t');
            if (s.empty()) break;
            Transaction t(s.c_str());
            transactions.emplace_back(t);
            transactions[n].linkNext(transactions[n + 1]);
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
        file.close();
        stats();
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
            socklen_t addressSize = sizeof(serverAddressStorage);
            ssize_t n = recvfrom(backendSocket, buffer, sizeof(buffer), 0,
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
