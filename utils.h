#ifndef UTILS_H
#define UTILS_H


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
    cout << "UDP send: " << s << " " << s.length() << endl;
    UDPSendPrimitive(senderSocket, receiverAddress, receiverAddressSize, s);
}

template<class T>
void TCPSendObject(int senderSocket, const T &t) {
    string s = t.toString();
    cout << "TCP send: " << s << " " << s.length() << endl;
    TCPSendPrimitive(senderSocket, s);
}

template<class T>
T UDPReceiveObject(int receiverSocket, sockaddr *senderAddress = nullptr,
                   socklen_t *senderAddressSize = nullptr) {
    string s = UDPReceiveString(receiverSocket, senderAddress, senderAddressSize);
    cout << "UDP receive: " << s << " " << s.length() << endl;
    return T(s);
}

template<class T>
T TCPReceiveObject(int receiverSocket) {
    string s = TCPReceiveString(receiverSocket);
    cout << "TCP receive: " << s << " " << s.length() << endl;
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

#endif//UTILS_H
