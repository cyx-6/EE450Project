#include "backend.h"

int main() {
    Backend backend("C", "block3.txt", 23062);
    backend.start();
    return 0;
}