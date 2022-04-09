#ifndef USER_H
#define USER_H

#include <string>
#include <utility>

using namespace std;

class User{
private:
    long int ranking, nextRanking, transactionNumber, balance;
    string userName;
public:
    explicit User(long int ranking, string userName, long int transactionNumber,
                  long int balance, long int nextRanking = 0) :
                  ranking(ranking), userName(std::move(userName)),
                  transactionNumber(transactionNumber),
                  balance(balance), nextRanking(nextRanking) {}

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
        if (token == nullptr) nextRanking = 0;
        else nextRanking = strtol(token, nullptr, 10);
        token = strtok(nullptr, "\t");
        assert(token == nullptr);
    }

    int encode(char* buffer, bool withNextRanking = true) {
        string s = to_string(ranking) + "\t" + userName + "\t" +
                to_string(transactionNumber) + "\t" + to_string(balance) +
                (withNextRanking ? "\t" + to_string(nextRanking) : "");
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

    void setFirst() {
        ranking = 1;
    }

    void linkNext(User &u) {
        assert(this != &u);
        u.ranking = nextRanking = ranking + 1;
    }

    static bool comp(const User& u, const User& v) {
        return u.transactionNumber > v.transactionNumber;
    }
};

#endif //USER_H
