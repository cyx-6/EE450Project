#ifndef UTILS_H
#define UTILS_H


#include "config.h"

template<class T>
void UDPSendPrimitive(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, const T &t) {
    string s = to_string(t);
    assert(sendto(senderSocket, s.c_str(), s.length(), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

void UDPSendPrimitive(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, const string &s) {
    assert(sendto(senderSocket, s.c_str(), s.length(), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

template<class T>
void TCPSendPrimitive(int senderSocket, const T &t) {
    string s = to_string(t);
    assert(send(senderSocket, s.c_str(), s.length(), 0) != -1);
}

void TCPSendPrimitive(int senderSocket, const string &s) {
    assert(send(senderSocket, s.c_str(), s.length(), 0) != -1);
}

long int UDPReceiveLongInt(int receiverSocket, sockaddr *senderAddress = nullptr,
                           socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return strtol(buffer, nullptr, 10);
}

long int TCPReceiveLongInt(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    return strtol(buffer, nullptr, 10);
}

string UDPReceiveString(int receiverSocket, sockaddr *senderAddress = nullptr,
                        socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return buffer;
}

string TCPReceiveString(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    return buffer;
}

template <class T>
void UDPSendObject(int senderSocket, sockaddr *receiverAddress,
                   socklen_t receiverAddressSize, const T &t) {
    string s = t.toString();
    cout << "UDP send: " << s << " " << s.length() << endl;
    assert(sendto(senderSocket, s.c_str(), s.length(), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

template <class T>
void TCPSendObject(int senderSocket, const T &t) {
    string s = t.toString();
    cout << "TCP send: " << s << " " << s.length() << endl;
    assert(send(senderSocket, s.c_str(), s.length(), 0) != -1);
}

template <class T>
T UDPReceiveObject(int receiverSocket, sockaddr *senderAddress = nullptr,
                   socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    cout << "UDP receive: " << string(buffer) << " " << string(buffer).length() << endl;
    return T(buffer);
}

template <class T>
T TCPReceiveObject(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    cout << "start" << endl;
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    cout << "TCP receive: " << string(buffer) << " " << string(buffer).length() << endl;
    return T(buffer);
}

#endif//UTILS_H
