#include <string>
#include <thread>
#include <stdio.h>

using namespace std;

void task1(string msg) {
    printf("task1 says: %s\n", msg.c_str());
}

int main() {
    thread t1(task1, "Hello World!");
    t1.join();
}