#ifndef CONFIG_H
#define CONFIG_H


class Config {
public:
    static const int BUFFER_LEN = 4096;
    static const int BUFFER_SIZE = sizeof(char) * BUFFER_LEN;
    static const int INITIAL_BALANCE = 1000;
    static constexpr const char *LOCALHOST = "127.0.0.1";
    static constexpr const char *TXLIST_FILE = "alichain.txt";
    static const char SEPARATOR = '|';
};


#endif//CONFIG_H
