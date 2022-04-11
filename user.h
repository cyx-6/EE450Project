#ifndef USER_H
#define USER_H

#include <string>
#include <utility>

#include "config.h"

using namespace std;

class User {
public:
    explicit User(long int ranking, string userName, long int transactionNumber,
                  long int balance) :
            ranking(ranking), userName(std::move(userName)),
            transactionNumber(transactionNumber),
            balance(balance), initialBalanceAdded(false) {}

    explicit User(const char *u) {
        char s[strlen(u)];
        strcpy(s, u);
        cout << "fff" << string(u) << endl;
        cout << "ggg" << string(s) << endl;
        char *token = strtok(s, Config::SEPARATOR);
        assert(token != nullptr);
        ranking = strtol(token, nullptr, 10);
        token = strtok(nullptr, Config::SEPARATOR);
        assert(token != nullptr);
        userName = string(token);
        token = strtok(nullptr, Config::SEPARATOR);
        assert(token != nullptr);
        transactionNumber = strtol(token, nullptr, 10);
        token = strtok(nullptr, Config::SEPARATOR);
        assert(token != nullptr);
        balance = strtol(token, nullptr, 10);
        token = strtok(nullptr, Config::SEPARATOR);
        assert(token != nullptr);
        initialBalanceAdded = strtol(token, nullptr, 10);
        token = strtok(nullptr, Config::SEPARATOR);
        assert(token == nullptr);
    }

    string toString() const {
        return to_string(ranking) + Config::SEPARATOR + userName + Config::SEPARATOR +
               to_string(transactionNumber) + Config::SEPARATOR + to_string(balance) +
               Config::SEPARATOR + to_string(initialBalanceAdded);
    }

//    int encode(char *buffer) const {
//        string s = toString();
//        strcpy(buffer, s.c_str());
//        return 0;
//    }

    void merge(const User &u) {
        transactionNumber += u.transactionNumber;
        balance += u.balance;
    }

    string getUserName() const {
        return userName;
    }

    void setRanking(long int r) {
        ranking = r;
    }

    static bool comp(const User &u, const User &v) {
        return u.transactionNumber > v.transactionNumber;
    }

    bool exist() const {
        return transactionNumber > 0;
    }

    void addInitialBalance() {
        assert(!initialBalanceAdded);
        assert(transactionNumber);
        balance += Config::INITIAL_BALANCE;
        initialBalanceAdded = true;
    }

    bool transferable(const Operation &o) const {
        return initialBalanceAdded ? (balance >= o.getTransferAmount()) :
                                   (balance + Config::INITIAL_BALANCE >= o.getTransferAmount());
    }

    void transfer(User &u, const Operation &o) {
        assert(transferable(o));
        long int t = o.getTransferAmount();
        balance -= t;
        u.balance += t;
    }

    long int currentBalance() const {
        return initialBalanceAdded ? balance : (balance + Config::INITIAL_BALANCE);
    }

private:
    long int ranking, transactionNumber, balance;
    string userName;
    bool initialBalanceAdded;
};

#endif //USER_H
