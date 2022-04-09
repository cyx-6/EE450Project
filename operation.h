#ifndef OPERATION_H
#define OPERATION_H


using namespace std;

class Operation{
private:
    enum Type{NONE = 0, CHECK_WALLET, TXCOINS, TXLIST, STATS};
    Type type = NONE;
    string userName1, userName2;
    long int transferAmount = 0;
public:
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

    int encode(char* buffer) {
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
};


#endif //OPERATION_H
