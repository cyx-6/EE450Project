#ifndef USER_H
#define USER_H

#include <string>
#include <utility>

using namespace std;

class User{
public:
    explicit User(long int ranking, string userName, long int transactionNumber,
                  long int balance) :
                  ranking(ranking), userName(std::move(userName)),
                  transactionNumber(transactionNumber),
                  balance(balance) {}

    explicit User(const char* u) {
        char s[strlen(u)];
        strcpy(s, u);
        char* token = strtok(s, "\t");
        assert(token != nullptr);
        ranking = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        userName = string(token);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        transactionNumber = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token != nullptr);
        balance = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token == nullptr);
    }

    int encode(char* buffer, bool withNextRanking = true) const {
        string s = to_string(ranking) + "\t" + userName + "\t" +
                to_string(transactionNumber) + "\t" + to_string(balance);
        strcpy(buffer, s.c_str());
        return 0;
    }

    void merge(const User &u) {
        transactionNumber += u.transactionNumber;
        balance += u.balance;
    }

    string getUserName() {
        return userName;
    }

    void setRanking(long int r) {
        ranking = r;
    }

    static bool comp(const User& u, const User& v) {
        return u.transactionNumber > v.transactionNumber;
    }

private:
    long int ranking, transactionNumber, balance;
    string userName;
};

#endif //USER_H
