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
#include <vector>
//#include <utility>

#include "config.h"
#include "operation.h"
#include "user.h"
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

    static void checkWallet(int clientSocket, const string &userName) {
        cout << userName + " sent a balance enquiry request to the main server." << endl;
        User u = TCPReceiveObject<User>(clientSocket);
        if (u.exist())
            cout << "The current balance of " + userName + " is : " +
                            to_string(u.currentBalance()) + " alicoins." << endl;
        else
            cout << "“Unable to proceed with the balance enquiry request as " + userName +
                    " is not part of the network." << endl;
    };

    static void TXCoins(int clientSocket, const Operation& o) {
        cout << o.getUserName1() + " has requested to transfer" + to_string(o.getTransferAmount()) +
                        " coins to " + o.getUserName2() << "." << endl;
        User u = TCPReceiveObject<User>(clientSocket),
                v = TCPReceiveObject<User>(clientSocket);
        if (!u.exist() && !v.exist())
            cout << "Unable to proceed with the transaction as " +
                            o.getUserName1() + " and " + o.getUserName2() +
                            " are not part of the network." << endl;
        else if (!u.exist() || !v.exist())
            cout << "Unable to proceed with the transaction as " +
                            (u.exist() ? o.getUserName2() : o.getUserName1()) +
                            " is not part of the network." << endl;
        else if (!u.transferable(o))
            cout << o.getUserName1() + " was unable to transfer" +
                            to_string(o.getTransferAmount()) + " alicoins to" +
                            o.getUserName2() + " because of insufficient balance. The current balance of " +
                            o.getUserName1() + " is :" + to_string(u.currentBalance()) +
                            " alicoins." << endl;
        else {
            u.transfer(v, o);
            cout << o.getUserName1() + " successfully transferred" +
                            to_string(o.getTransferAmount()) + " alicoins to" +
                            o.getUserName2() + ". The current balance of " +
                            o.getUserName1() + " is :" + to_string(u.currentBalance()) +
                            " alicoins." << endl;
        }
    };

    void TXList() {
        cout << clientName + " sent a sorted list request to the main server." << endl;
    };

    static void stats(int clientSocket, const string &userName) {
        cout << userName + " sent a statistics enquiry request to the main server." << endl;
        long int n = TCPReceiveLongInt(clientSocket);
        vector<User> users;
        users.reserve(n);
        for (int i = 0; i < n; ++i) {
            User u = TCPReceiveObject<User>(clientSocket);
            users.emplace_back(u);
        }
        cout << userName + " statistics are the following.:" << endl <<
                "Rank--Username--NumofTransacions--Total" << endl;
        for (const User &u : users)
            cout << u.toString() << endl;

    };

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
        TCPSendPrimitive(clientSocket, clientName);
        TCPSendObject(clientSocket, o);
        switch (o.getType()) {
            case Operation::Type::CHECK_WALLET:
                checkWallet(clientSocket, o.getUserName1());
                break;
            case Operation::Type::TXCOINS:
                TXCoins(clientSocket, o);
                break;
            case Operation::Type::TXLIST:
                TXList();
                break;
            case Operation::Type::STATS:
                stats(clientSocket, o.getUserName1());
                break;
            default:
                assert(false);
        }
        close(clientSocket);
        return 0;
    }
};

#endif//CLIENT_H
