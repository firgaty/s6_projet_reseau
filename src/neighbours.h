#ifndef NEIGHBOURS_H
#define NEIGHBOURS_H

#include <time.h>
#include <inttypes.h>

typedef struct neighbours_t {
  unsigned char* ip;
  uint16_t port;
  time_t last_hello;
  time_t last_hello_long;
}
neighbours_t;

#endif  // !NEIGHBOURS_H