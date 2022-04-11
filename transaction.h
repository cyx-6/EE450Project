#ifndef TRANSACTION_H
#define TRANSACTION_H


#include <string>
#include <utility>
#include <vector>

#include "utils.h"

using namespace std;

class Transaction {
public:
    explicit Transaction(int serialID, string userName1,
                         string userName2, int transferAmount) : serialID(serialID), userName1(std::move(userName1)),
                                                                 userName2(std::move(userName2)), transferAmount(transferAmount) {}

    explicit Transaction(const string &s) {
        vector<string> v = stringToList(s);
        assert(v.size() == 4);
        serialID = stoi(v[0]);
        userName1 = v[1];
        userName2 = v[2];
        transferAmount = stoi(v[3]);
    }

    string toString() const {
        return listToString({to_string(serialID), userName1, userName2, to_string(transferAmount)});
    }

    string getUserName1() const {
        return userName1;
    }

    string getUserName2() const {
        return userName2;
    }

    int getTransferAmount() const {
        return transferAmount;
    }

    int getSerialID() const {
        return serialID;
    }

    static bool comp(const Transaction &u, const Transaction &v) {
        return u.serialID < v.serialID;
    }

private:
    int serialID, transferAmount;
    string userName1, userName2;
};
#endif//TRANSACTION_H
