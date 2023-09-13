#include <atomic>
#include <stdio.h>

std::atomic<int> a;
std::atomic<int> b;

int main(int argc, char** argv) {
    a = 10;
    b = a.load();
    fprintf(stdout, "%d\n", b.load());
    return 0;
}