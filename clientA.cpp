//#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char **argv) {
    Client client("A", 25062);
    client.run(argc, argv);
    return 0;
}
