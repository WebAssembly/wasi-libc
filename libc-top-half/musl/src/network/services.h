#ifndef SERVICES_H
#define SERVICES_H
#include "lookup.h"
#include <uthash.h>

typedef struct services_entry_s {
  char *name;
  struct service *svcs;
  UT_hash_handle hh;
} services_entry_t;

void init_services();

extern services_entry_t *services;

#endif // SERVICES_H
