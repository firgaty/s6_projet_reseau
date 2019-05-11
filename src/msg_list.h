#ifndef MSG_LIST_H_
#define MSG_LIST_H_

#include <pthread.h>

#include "dllist.h"
#include "shared_resources.h"
#include "types.h"

bool is_in_msg_list(data_body_t* b);
bool add_msg(data_body_t* b);

/**
 * @brief Removes the n oldest messages.
 *
 * @param n Number of messages to remove.
 */
void rm_n_msg(size_t n);
bool rm_msg();

#endif  // !MSG_LIST_H_
