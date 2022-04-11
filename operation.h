#ifndef OPERATION_H
#define OPERATION_H

#include "transaction.h"
#include "utils.h"

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
                transferAmount = stoi(string(argv[3]));
                serialID = 0;
                return;
            default:
                assert(false);
        }
    }

    explicit Operation(const string &s) {
        vector<string> v = stringToList(s);
        assert(!v.empty() && v[0].length() == 1);
        switch (v[0][0]) {
            case '0':
                assert(v.size() == 1);
                return;
            case '1':
                assert(v.size() == 2);
                type = CHECK_WALLET;
                userName1 = v[1];
                return;
            case '2':
                assert(v.size() == 5);
                type = TXCOINS;
                userName1 = v[1];
                userName2 = v[2];
                transferAmount = stoi(v[3]);
                serialID = stoi(v[4]);
                return;
            case '3':
                assert(v.size() == 1);
                type = TXLIST;
                return;
            case '4':
                assert(v.size() == 2);
                type = STATS;
                userName1 = v[1];
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

    void setSerialID(int sid) {
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

    int getTransferAmount() const {
        assert(type == TXCOINS);
        return transferAmount;
    }

    string toString() const {
        switch (type) {
            case NONE:
                return listToString({"0"});
            case CHECK_WALLET:
                return listToString({"1", userName1});
            case TXCOINS:
                return listToString({"2", userName1, userName2, to_string(transferAmount), to_string(serialID)});
            case TXLIST:
                return listToString({"3"});
            case STATS:
                return listToString({"4", userName1});
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
    int transferAmount = 0, serialID = 0;

};


#endif //OPERATION_H
