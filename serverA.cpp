#include "backend.h"

int main() {
    Backend backend("A", "block1.txt", 21062, 24062);
    backend.start();
    return 0;
}