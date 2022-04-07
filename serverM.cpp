#include "server.h"

using namespace std;

int main()
{
    Server server({"25062", "26062"});
    server.start();
    return 0;
}

