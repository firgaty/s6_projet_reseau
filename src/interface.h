#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <pthread.h>
#include <stdio.h>
#include <readline/readline.h>
#include <string.h>

#include "types.h"


void *listen_input();
int16_t process_input(char *input);


#endif // !INTERFACE_H_