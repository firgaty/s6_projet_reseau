/**
 * @file main.c
 * @author Félix Desmaretz
 * @brief
 * @version 0.1
 * @date 2019-03-11
 *
 * @copyright Copyright (c) 2019
 *
 */
#include <stdio.h>
#include <pthread.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#include "connect.h"
#include "msg.h"
#include "msg_list.h"
#include "shared_resources.h"
#include "test.h"
#include "interface.h"
#include "types.h"
#include "serialization.h"
// #include "gui.h"
#include "worker.h"

int main(int argc, char *argv[]) {
  // Initialisation du générateur de nombres aléatoires.
  srand((unsigned)time(NULL));
  new_client_id();

  //set_server_port((uint16_t)atoi(argv[1]));
  set_server_port(4243);

  char* line;

  while (1) {
    line = readline("Input alias: ");
    if (strlen(line) > 0 && strlen(line) < 1024 - 1) {
      new_client_name(line, strlen(line));
      free(line);
      break;
    }
    free(line);
  }

  // pthread_t thread_id[3];
  pthread_t srv;
  pthread_t worker;
  pthread_t gui;

  printf("Thread_creation...\n");

  pthread_create(&srv, NULL, udp_server, NULL);
  printf("SRV end.\n");
  pthread_create(&worker, NULL, worker_loop, NULL);
  pthread_create(&gui, NULL, init_gui, NULL);
  //pthread_create(&thread_id[2], NULL, listen_input, (void*)&thread_id[2]);

  pthread_join(gui, NULL);

  // test_connect((char*)argv[2]);
  
  test_connect("1212");

  listen_input();

  printf("End of threads...\n");
  // start_server(1);
  // test_msg();
  // test_dllist();
  // test_neighbour_map();

  return (0);
}
