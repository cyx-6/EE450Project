//#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char** argv)
{
    Client client("B", 26062);
    client.run(argc, argv);
    return 0;
}
