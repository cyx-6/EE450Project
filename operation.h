#ifndef OPERATION_H
#define OPERATION_H

#include "transaction.h"

using namespace std;

class Operation {
public:
    enum Type {
        NONE = 0, CHECK_WALLET, TXCOINS, TXLIST, STATS
    };

    Operation() = default;

    explicit Operation(int argc, char **argv) {
        switch (argc) {
            case 2:
                if (strcmp(argv[1], "TXLIST") == 0) {
                    type = TXLIST;
                } else {
                    type = CHECK_WALLET;
                    userName1 = string(argv[1]);
                }
                return;
            case 3:
                type = STATS;
                userName1 = string(argv[1]);
                return;
            case 4:
                type = TXCOINS;
                userName1 = string(argv[1]);
                userName2 = string(argv[2]);
                transferAmount = strtol(argv[3], nullptr, 10);
                serialID = 0;
                return;
            default:
                assert(false);
        }
    }

    explicit Operation(char *s) {
        char *token = strtok(s, Config::SEPARATOR);
        assert(token != nullptr);
        switch (token[0]) {
            case '0':
                return;
            case '1':
                type = CHECK_WALLET;
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                userName1 = string(token);
                return;
            case '2':
                type = TXCOINS;
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                userName1 = string(token);
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                userName2 = string(token);
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                transferAmount = strtol(token, nullptr, 10);
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                serialID = strtol(token, nullptr, 10);
                return;
            case '3':
                type = TXLIST;
                return;
            case '4':
                type = STATS;
                token = strtok(nullptr, Config::SEPARATOR);
                assert(token != nullptr);
                userName1 = string(token);
                return;
            default:
                assert(false);
        }
    }

    pair<Operation, Operation> toSubOperation() const {
        assert(type == TXCOINS);
        Operation u, v;
        u.type = v.type = CHECK_WALLET;
        u.userName1 = userName1;
        v.userName1 = userName2;
        return make_pair(u, v);
    }

    Transaction toTransaction() const {
        assert(type == TXCOINS);
        return Transaction(serialID, userName1, userName2, transferAmount);
    }

    void setSerialID(long int sid) {
        serialID = sid;
    }

    Type getType() const {
        return type;
    }

    string getUserName1() const {
        assert(type != TXLIST);
        return userName1;
    }

    string getUserName2() const {
        assert(type == TXCOINS);
        return userName2;
    }

    long int getTransferAmount() const {
        assert(type == TXCOINS);
        return transferAmount;
    }

    string toString() const {
        switch (type) {
            case NONE:
                return "0";
            case CHECK_WALLET:
                return "1" + string(Config::SEPARATOR) + userName1;
            case TXCOINS:
                return "2" + string(Config::SEPARATOR) + userName1 + Config::SEPARATOR + userName2 +
                       Config::SEPARATOR + to_string(transferAmount);
            case TXLIST:
                return "3";
            case STATS:
                return "4" + string(Config::SEPARATOR) + userName1;
            default:
                assert(false);
        }
    }

//    int encode(char *buffer) const {
//        string s = toString();
//        strcpy(buffer, s.c_str());
//        return 0;
//    }

private:
    Type type = NONE;
    string userName1, userName2;
    long int transferAmount = 0, serialID = 0;

};


#endif //OPERATION_H
