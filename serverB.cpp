#include "backend.h"

int main() {
    Backend backend("B", "block2.txt", 22062);
    backend.start();
    return 0;
}