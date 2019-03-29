#ifndef NEIGHBOUR_H
#define NEIGHBOUR_H

#include <inttypes.h>
#include <time.h>

typedef struct neighbour_t {
  unsigned char* ip;
  uint16_t port;
  time_t last_hello;
  time_t last_hello_long;
} neighbour_t;

#endif  // !NEIGHBOUR_H