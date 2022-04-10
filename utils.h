#ifndef UTILS_H
#define UTILS_H


#include "config.h"
#include "operation.h"
#include "transaction.h"
#include "user.h"

template<class T>
void UDPSendPrimitive(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, T t) {
    string s = to_string(t);
    assert(sendto(senderSocket, s.c_str(), sizeof(s.c_str()), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

template<class T>
void TCPSendPrimitive(int senderSocket, T t) {
    string s = to_string(t);
    assert(send(senderSocket, s.c_str(), sizeof(s.c_str()), 0) != -1);
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

void UDPSendOperation(int senderSocket, sockaddr *receiverAddress,
                      socklen_t receiverAddressSize, const Operation &o) {
    char buffer[Config::BUFFER_LEN];
    o.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

void TCPSendOperation(int senderSocket, const Operation &o) {
    char buffer[Config::BUFFER_LEN];
    o.encode(buffer);
    assert(send(senderSocket, buffer, sizeof(buffer), 0) != -1);
}

Operation UDPReceiveOperation(int receiverSocket, sockaddr *senderAddress = nullptr,
                              socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return Operation(buffer);
}

Operation TCPReceiveOperation(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    return Operation(buffer);
}

void UDPSendTransaction(int senderSocket, sockaddr *receiverAddress,
                        socklen_t receiverAddressSize, const Transaction &t) {
    char buffer[Config::BUFFER_LEN];
    t.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

void TCPSendTransaction(int senderSocket, const Transaction &t) {
    char buffer[Config::BUFFER_LEN];
    t.encode(buffer);
    assert(send(senderSocket, buffer, sizeof(buffer), 0) != -1);
}

Transaction UDPReceiveTransaction(int receiverSocket, sockaddr *senderAddress = nullptr,
                                  socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return Transaction(buffer);
}

Transaction TCPReceiveTransaction(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    return Transaction(buffer);
}

void UDPSendUser(int senderSocket, sockaddr *receiverAddress,
                 socklen_t receiverAddressSize, const User &u) {
    char buffer[Config::BUFFER_LEN];
    u.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

void TCPSendUser(int senderSocket, const User &u) {
    char buffer[Config::BUFFER_LEN];
    u.encode(buffer);
    assert(send(senderSocket, buffer, sizeof(buffer), 0) != -1);
}

User UDPReceiveUser(int receiverSocket, sockaddr *senderAddress = nullptr,
                    socklen_t *senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return User(buffer);
}

User TCPReceiveUser(int receiverSocket) {
    char buffer[Config::BUFFER_LEN];
    assert(recv(receiverSocket, buffer, Config::BUFFER_SIZE, 0) != -1);
    return User(buffer);
}

#endif//UTILS_H
