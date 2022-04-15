#include "client.h"

using namespace std;

int main(int argc, char **argv) {
    Client client("B", 26062);
    client.start(argc, argv);
    return 0;
}
