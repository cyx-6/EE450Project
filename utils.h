#ifndef UTILS_H
#define UTILS_H


#include "operation.h"
#include "user.h"
#include "transaction.h"
#include "config.h"

template <class T>
void UDPSendPrimitive(int senderSocket, sockaddr* receiverAddress,
                    socklen_t receiverAddressSize, T t) {
    string s = to_string(t);
    assert(sendto(senderSocket, s.c_str(), sizeof(s.c_str()), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

long int UDPReceiveLongInt(int receiverSocket, sockaddr* senderAddress = nullptr,
                           socklen_t* senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return strtol(buffer, nullptr, 10);
}

void UDPSendOperation(int senderSocket, sockaddr* receiverAddress,
                    socklen_t receiverAddressSize, const Operation& o) {
    char buffer[Config::BUFFER_LEN];
    o.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

Operation UDPReceiveOperation(int receiverSocket, sockaddr* senderAddress = nullptr,
                              socklen_t* senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return Operation(buffer);
}

void UDPSendTransaction(int senderSocket, sockaddr* receiverAddress,
                        socklen_t receiverAddressSize, const Transaction& t) {
    char buffer[Config::BUFFER_LEN];
    t.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

Transaction UDPReceiveTransaction(int receiverSocket, sockaddr* senderAddress = nullptr,
                                  socklen_t* senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return Transaction(buffer);
}

void UDPSendUser(int senderSocket, sockaddr* receiverAddress,
                      socklen_t receiverAddressSize, const User& u) {
    char buffer[Config::BUFFER_LEN];
    u.encode(buffer);
    assert(sendto(senderSocket, buffer, sizeof(buffer), 0,
                  receiverAddress, receiverAddressSize) != -1);
}

User UDPReceiveUser(int receiverSocket, sockaddr* senderAddress = nullptr,
                    socklen_t* senderAddressSize = nullptr) {
    char buffer[Config::BUFFER_LEN];
    assert(recvfrom(receiverSocket, buffer, Config::BUFFER_SIZE, 0,
                    senderAddress, senderAddressSize) != -1);
    return User(buffer);
}

#endif //UTILS_H
