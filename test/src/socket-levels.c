#include <sys/socket.h>

_Static_assert(SOL_TCP == 6, "SOL_TCP must match the socket API value");
_Static_assert(SOL_UDP == 17, "SOL_UDP must match the socket API value");

int main(void) { return 0; }
