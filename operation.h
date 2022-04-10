#ifndef OPERATION_H
#define OPERATION_H

#include "transaction.h"

using namespace std;

class Operation{
public:
    enum Type{NONE = 0, CHECK_WALLET, TXCOINS, TXLIST, STATS};

    Operation() = default;

    explicit Operation(int argc, char** argv) {
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

    explicit Operation(char* s) {
        char* token = strtok(s, "\t");
        assert(token != nullptr);
        switch (token[0]) {
            case '0':
                return;
            case '1':
                type = CHECK_WALLET;
                token = strtok(nullptr, "\t");
                assert(token != nullptr);
                userName1 = string(token);
                return;
            case '2':
                type = TXCOINS;
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
                assert(token != nullptr);
                serialID = strtol(token, nullptr, 10);
                return;
            case '3':
                type = TXLIST;
                return;
            case '4':
                type = STATS;
                token = strtok(nullptr, "\t");
                assert(token != nullptr);
                userName1 = string(token);
                return;
            default:
                assert(false);
        }
    }

    pair<Operation, Operation> toSubOperation() {
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

    long int getTransferAmount() {
        assert(type == TXCOINS);
        return transferAmount;
    }

    int encode(char* buffer) const {
        string s;
        switch (type) {
            case NONE:
                s = "0";
                break;
            case CHECK_WALLET:
                s = "1\t" + userName1;
                break;
            case TXCOINS:
                s = "2\t" + userName1 + "\t" + userName2 + "\t" + to_string(transferAmount);
                break;
            case TXLIST:
                s = "3";
                break;
            case STATS:
                s = "4\t" + userName1;
                break;
            default:
                assert(false);
        }
        strcpy(buffer, s.c_str());
        return 0;
    }

    void print() {
        cout << type << " " << userName1 << " " << userName2 << " " << transferAmount << endl;
    }

private:
    Type type = NONE;
    string userName1, userName2;
    long int transferAmount = 0, serialID = 0;

};


#endif //OPERATION_H
