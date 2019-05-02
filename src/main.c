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
#include <stdlib.h>
#include <time.h>

#include "msg.h"
#include "test.h"
#include "connect.h"

int main(int argc, char const *argv[]) {

	// Initialisation du générateur de nombres aléatoires.
  srand((unsigned)time(NULL));

  printf("%s", random_stream(8));

  // test_msg();
  // test_dllist();
  // test_neighbour_map();

  return (0);
}
