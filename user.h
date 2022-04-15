#ifndef USER_H
#define USER_H

#include <string>
#include <utility>

#include "config.h"
#include "utils.h"

using namespace std;

class User {
public:
    explicit User(const string &s) {
        vector<string> v = stringToList(s);
        assert(v.size() == 5);
        ranking = stoi(v[0]);
        userName = v[1];
        transactionNumber = stoi(v[2]);
        balance = stoi(v[3]);
        initialBalanceAdded = stoi(v[4]);
    }

    static User initialUser(const string &username) {
        return User(0, username, 0, 0);
    }

    string toString() const {
        return listToString({to_string(ranking), userName, to_string(transactionNumber),
                             to_string(balance), to_string(initialBalanceAdded)});
    }

    void merge(const User &u) {
        transactionNumber += u.transactionNumber;
        balance += u.balance;
    }

    string getUserName() const {
        return userName;
    }

    void setRanking(int r) {
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
        return initialBalanceAdded ? (balance >= o.getTransferAmount()) : (balance + Config::INITIAL_BALANCE >= o.getTransferAmount());
    }

    void transfer(User &u, const Operation &o) {
        assert(transferable(o));
        int t = o.getTransferAmount();
        balance -= t;
        ++transactionNumber;
        u.balance += t;
        ++u.transactionNumber;
    }

    int currentBalance() const {
        return initialBalanceAdded ? balance : (balance + Config::INITIAL_BALANCE);
    }

    static pair<User, User> fromTransaction(const Transaction &t) {
        int d = t.getTransferAmount();
        return make_pair(User(0, t.getUserName1(), 1, -d),
                         User(0, t.getUserName2(), 1, d));
    }

private:
    explicit User(int ranking, string userName,
                  int transactionNumber, int balance) : ranking(ranking), userName(std::move(userName)),
                                                        transactionNumber(transactionNumber),
                                                        balance(balance), initialBalanceAdded(false) {}

    int ranking, transactionNumber, balance;
    string userName;
    bool initialBalanceAdded;
};

#endif//USER_H
