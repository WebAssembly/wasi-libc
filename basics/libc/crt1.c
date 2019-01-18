extern int main(int, char *[]);
void exit(int) __attribute__((__noreturn__));

void _start(void) {
    // For now, just call main with no arguments.
    int r = main(0, 0);
    if (r != 0) {
        exit(r);
    }
}
