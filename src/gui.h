#ifndef GUI_H_
# define GUI_H_

#include <gtk/gtk.h>
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

#define MSG_USER 0
#define MSG_INFO 1

// GtkWidget *g_label_infos;
GtkWidget *window;
GtkWidget *g_text_view_messages;
GtkTextBuffer *g_text_view_messages_buffer;
GtkWidget *g_entry_message;
GtkWidget *g_button_send;

void *init_gui();

void on_window_main_destroy();
void on_entry_message_activate();
void on_button_send_clicked();

size_t nick_len(const char *data, size_t data_len);
void update_title(size_t nsym);
void print_message(const char *data, size_t len, size_t type);
void send_message();



#endif