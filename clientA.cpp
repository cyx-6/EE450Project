#include "client.h"

using namespace std;

int main(int argc, char **argv) {
    Client client("A", 25062);
    client.start(argc, argv);
    return 0;
}
