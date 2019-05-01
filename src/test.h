#ifndef TEST_H_
#define TEST_H_

#include "types.h"
#include "msg.h"
#include "dllist.h"
#include "neighbour_map.h"
#include "serialization.h"

/**
 * @brief Test for msg.h functions.
 * 
 * @return int 0 if OK else -1.
 */
int test_msg();
int test_dllist();
int test_neighbour_map();
#endif // TEST_H_
