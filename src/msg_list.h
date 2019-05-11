#ifndef MSG_LIST_H_
#define MSG_LIST_H_

#include <pthread.h>

#include "dllist.h"
#include "types.h"
#include "shared_resources.h"

void *list_loop();


bool is_in_msg_list(data_body_t *m);
bool add_msg(data_body_t *m);
dll_msg_t *get_dll_msg(data_body_t *m);

#endif  // !MSG_LIST_H_
