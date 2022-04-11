#include "server.h"

using namespace std;

int main() {
    Server server({25062, 26062},
                  24062,
//                  {21062, 22062, 23062});
                  {21062});
    server.start();
    return 0;
}
