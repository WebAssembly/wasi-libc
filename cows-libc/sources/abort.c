void abort(void) {
    // COWS doesn't yet support signals, so just trap to halt the program.
    __builtin_trap();
}
