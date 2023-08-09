#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define EXPORT __attribute__((visibility("default"))) __attribute__((used))
#define WEAK __attribute__((__weak__))

static const char *error = 0;

EXPORT WEAK int dlclose(void *library) {
    error = "dlclose not implemented";
    return -1;
}

EXPORT WEAK char *dlerror(void) {
    const char *var = error;
    error = 0;
    return (char*) var;
}

EXPORT WEAK void *dlopen(const char *name, int flags) {
    error = "dlopen not implemented";
    return 0;
}

EXPORT WEAK void *dlsym(void *library, const char *name) {
    error = "dlsym not implemented";
    return 0;
}
