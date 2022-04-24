#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#include "config.h"

using namespace std;

void UDPSendPrimitive(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, const string &s) {
    assert(sendto(senderSocket, s.c_str(), s.length(), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

template<class T>
void UDPSendPrimitive(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, const T &t) {
    string s = to_string(t);
    UDPSendPrimitive(senderSocket, receiverAddress, receiverAddressSize, s);
}

void TCPSendPrimitive(int senderSocket, const string &s) {
    assert(send(senderSocket, s.c_str(), s.length(), 0) != -1);
}

template<class T>
void TCPSendPrimitive(int senderSocket, const T &t) {
    string s = to_string(t);
    TCPSendPrimitive(senderSocket, s);
}

string UDPReceiveString(int receiverSocket, sockaddr *senderAddress = nullptr,
                        socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    ssize_t n = recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                         senderAddress, senderAddressSize);
    assert(n != -1);
    buffer[n] = '\0';
    return buffer;
}

string TCPReceiveString(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    ssize_t n = recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0);
    assert(n != -1);
    buffer[n] = '\0';
    return buffer;
}

int UDPReceiveInt(int receiverSocket, sockaddr *senderAddress = nullptr,
                  socklen_t *senderAddressSize = nullptr) {
    string s = UDPReceiveString(receiverSocket, senderAddress, senderAddressSize);
    return stoi(s);
}

int TCPReceiveInt(int receiverSocket) {
    string s = TCPReceiveString(receiverSocket);
    return stoi(s);
}


template<class T>
void UDPSendObject(int senderSocket, sockaddr *receiverAddress,
                   socklen_t receiverAddressSize, const T &t) {
    string s = t.toString();
    cout << "\033[32m" << "UDP send: " << s << " (Length: " << s.length() << ")\033[0m" << endl;
    UDPSendPrimitive(senderSocket, receiverAddress, receiverAddressSize, s);
}

template<class T>
void TCPSendObject(int senderSocket, const T &t) {
    string s = t.toString();
    cout << "\033[32m" << "TCP send: " << s << " (Length: " << s.length() << ")\033[0m" << endl;
    TCPSendPrimitive(senderSocket, s);
}

template<class T>
T UDPReceiveObject(int receiverSocket, sockaddr *senderAddress = nullptr,
                   socklen_t *senderAddressSize = nullptr) {
    string s = UDPReceiveString(receiverSocket, senderAddress, senderAddressSize);
    cout << "\033[32m" << "UDP receive: " << s << " (Length: " << s.length() << ")\033[0m" << endl;
    return T(s);
}

template<class T>
T TCPReceiveObject(int receiverSocket) {
    string s = TCPReceiveString(receiverSocket);
    cout << "\033[32m" << "TCP receive: " << s << " (Length: " << s.length() << ")\033[0m" << endl;
    return T(s);
}

string listToString(const vector<string> &v) {
    string a;
    for (const string &s: v)
        a.append(s + Config::SEPARATOR);
    return a;
}

vector<string> stringToList(const string &s) {
    vector<string> v;
    string c = s;
    size_t p = c.find(Config::SEPARATOR);
    while (p != string::npos) {
        v.emplace_back(c.substr(0, p));
        c.erase(0, p + 1);
        p = c.find(Config::SEPARATOR);
    }
    return v;
}

sockaddr_in socketAddress(uint16_t port) {
    sockaddr_in address{};
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(Config::LOCALHOST);
    return address;
}

#endif//UTILS_H
