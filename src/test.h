#ifndef TEST_H_
#define TEST_H_

#include <wait.h>

#include "types.h"
#include "msg.h"
#include "dllist.h"
#include "neighbour_map.h"
#include "serialization.h"
#include "connect.h"

/**
 * @brief Test for msg.h functions.
 * 
 * @return int 0 if OK else -1.
 */
int test_msg();
int test_dllist();
int test_neighbour_map();
int test_connect(char *port);

#endif // TEST_H_
