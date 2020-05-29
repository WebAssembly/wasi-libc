// Each of the following complex functions can be implemented with a single
// wasm instruction, so use that implementation rather than the portable
// one in libm.

#include <complex.h>

float crealf(float _Complex x) {
    return __real__ x;
}

double creal(double _Complex x) {
    return __real__ x;
}

float cimagf(float _Complex x) {
    return __imag__ x;
}

double cimag(double _Complex x) {
    return __imag__ x;
}
