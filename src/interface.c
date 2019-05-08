#include "interface.h"

void* listen_input() {
  printf("Listen input...\n");
  char *line;
  int16_t status = 1;

  while (status > 0) {
    line = readline("> ");
    status = process_input(line);
    free(line);
  }
  // exit(0);
  return 0;
}

int16_t process_input(char* input) {
  size_t len = strlen(input);
  if (len <= 0)
    return 1;
  if (len >= 2) {
    /*
      options.
        q: exit
        i: add ip
        ...
    */
    if (input[0] == '/') {
      if (input[1] == 'q')
        return 0;
    }
     
    
  }
  // sinon print l'input.
  printf("%s\n", input);
  return 0;
}