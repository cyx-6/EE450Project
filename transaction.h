#ifndef TRANSACTION_H
#define TRANSACTION_H


#include <string>
#include <utility>

using namespace std;

class Transaction{
public:
    explicit Transaction(long int serialID, string userName1, string userName2,
                long int transferAmount) :
                serialID(serialID), userName1(std::move(userName1)), userName2(std::move(userName2)),
                transferAmount(transferAmount) {}

    explicit Transaction(const char* t) {
        char s[strlen(t)];
        strcpy(s, t);
        char* token = strtok(s, "\t");
        assert(token != nullptr);
        serialID = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        userName1 = string(token);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        userName2 = string(token);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        transferAmount = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token == nullptr);
    }

    int encode(char* buffer) const {
        string s = to_string(serialID) + "\t" + userName1 + "\t" + userName2 + "\t" +
                to_string(transferAmount);
        strcpy(buffer, s.c_str());
        return 0;
    }

    string getUserName1() const {
        return userName1;
    }

    string getUserName2() const {
        return userName2;
    }

    long int getTransferAmount() const {
        return transferAmount;
    }

    long int getSerialID() const {
        return serialID;
    }

    static bool comp(const Transaction& u, const Transaction& v) {
        return u.serialID < v.serialID;
    }

private:
    long int serialID, transferAmount;
    string userName1, userName2;
};
#endif //TRANSACTION_H
