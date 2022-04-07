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
#include <list>
#include <utility>
#include <poll.h>
//#include <utility>

using namespace std;

class Server {
private:
    list<string> TCPPortList;
    static void sigchld_handler(int s)
    {
        int saved_errno = errno;
        while(waitpid(-1, NULL, WNOHANG) > 0);
        errno = saved_errno;
    }

    static int TCPListener(const string& serverPort) {
        int TCPSocket;
        addrinfo hints{}, *serverInfo;
        int yes = 1;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        assert(getaddrinfo("localhost", serverPort.c_str(), &hints, &serverInfo) == 0);
        for (addrinfo *ai = serverInfo; ai != nullptr; ai = ai->ai_next) {
            TCPSocket = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (TCPSocket == -1) continue;
            assert(setsockopt(TCPSocket, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) != -1);
            int r = bind(TCPSocket, ai->ai_addr, ai->ai_addrlen);
            if (r == -1) {
                close(TCPSocket);
                continue;
            }
            assert(listen(TCPSocket, 8) != -1);
            struct sigaction sigAction{};
            sigAction.sa_handler = sigchld_handler; // reap all dead processes
            sigemptyset(&sigAction.sa_mask);
            sigAction.sa_flags = SA_RESTART;
            assert(sigaction(SIGCHLD, &sigAction, nullptr) != -1);
            return TCPSocket;
        }
        return -1;
    }


public:
    explicit Server(list<string> TCPPortList) : TCPPortList(std::move(TCPPortList)) {}
    void start() {
        pollfd polls[TCPPortList.size()];
        int TCPPortCount = 0;
        for (const string& port : TCPPortList) {
            polls[TCPPortCount].fd = TCPListener(port);
            polls[TCPPortCount].events = POLLIN;
            ++TCPPortCount;
        }
        while (true) {
            assert(poll(polls, TCPPortCount, -1) != -1);
            for (int i = 0; i < TCPPortCount; ++i) {
                if (polls[i].revents & POLLIN) {
                    sockaddr_storage clientAddressStorage{};
                    auto * clientAddress = (sockaddr*)&clientAddressStorage;
                    socklen_t addressSize = sizeof(clientAddressStorage);
                    int TCPSocket = accept(polls[i].fd, clientAddress, &addressSize);
                    assert(TCPSocket != -1);
                    char buffer[127];
                    ssize_t n = recv(TCPSocket, buffer, 127, 0);
                    cout << "server: " + string(buffer) << i << endl;
                    assert(n != -1);
                    buffer[n] = '\0';
                    assert(send(TCPSocket, buffer, sizeof(buffer), 0) != -1);
                    close(TCPSocket);
                }
            }
        }
    }
};

#endif //SERVER_H
