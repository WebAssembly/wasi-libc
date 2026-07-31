#include "tls_none.h"

static int state = 0;

int none_add(int a, int b) { return a + b; }

int none_state_get(void) { return state; }

void none_state_set(int value) { state = value; }
