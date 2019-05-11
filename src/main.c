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

#include <gtk/gtk.h>

// GtkWidget *g_label_infos;
GtkWidget *window;
GtkWidget *g_text_view_messages;
GtkWidget *g_text_view_messages_buffer;
GtkWidget *g_entry_message;
GtkWidget *g_button_send;

void init_gui(int argc, char *argv[]) {
  GtkBuilder *builder;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "glade/window_main.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gtk_builder_connect_signals(builder, NULL);
  // g_label_infos = GTK_WIDGET(gtk_builder_get_object(builder, "label_infos"));
  g_text_view_messages = GTK_WIDGET(gtk_builder_get_object(builder, "text_view_messages"));
  g_text_view_messages_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "text_view_messages_buffer"));
  g_entry_message = GTK_WIDGET(gtk_builder_get_object(builder, "entry_message"));
  g_button_send = GTK_WIDGET(gtk_builder_get_object(builder, "button_send"));

  g_object_unref(builder);

  gtk_widget_show(window);
  gtk_main();
}

int main(int argc, char *argv[]) {
		init_gui(argc, argv);
    return (EXIT_SUCCESS);
}

void on_entry_message_activate() {
	send_message();
}

void on_button_send_clicked() {
	send_message();
}

void on_window_main_destroy()
{
    gtk_main_quit();
}

// nsym: nombre de voisins symétriques
void update_title(size_t nsym) {
	char title[512];

	memset(title, 0, 512);
	snprintf(title, 512, "[%d] miaouchat", nsym);
	gtk_window_set_title(GTK_WINDOW(window), title);
}

// char *get_text() {
// 	GtkTextIter start, end;
// 	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view_messages));
// 	gchar *text;
// 	gtk_get_buffer_get_bounds(buffer, &start, &end);
// 	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
// 	return text;
// }

size_t nick_len(const char *data, size_t data_len) {
	size_t nlen;

	nlen = 0;
	while (nlen <= data_len && *data != ':') {
		nlen++;
		data++;
	}
	return nlen == data_len ? 0 : nlen;
}

void print_message(const char *data, size_t len, size_t type) {
	time_t rt;
	struct tm *t;
	size_t dlen;
	size_t nlen;
	char *d;
	GtkTextIter end;
	GtkTextTag *tag;

	time(&rt);
	t = localtime(&rt);

	// dlen = data[len - 1] == '\0' ? 10 + 1 + len + 1 : 10 + 1 + len + 2;
	// d = calloc(dlen, sizeof(char));
	// // comme un doute sur le +2..
	// snprintf(d + 10 + 1, len + 2, "%s\n", data);

	dlen = 10 + 1 + 1;
	d = calloc(dlen, sizeof(char));
	if (!d) {
		fprintf(stderr, "[!] Erreur d’allocation mémoire.");
	}
	strftime(d, dlen, "[%H:%M:%S] ", t);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(g_text_view_messages_buffer), NULL, "weight", PANGO_WEIGHT_LIGHT, NULL);
	gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1, tag, NULL);

	nlen = nick_len(data, len);
	dlen = 1 + nlen + 1 + 1 + 1;
	free(d);
	d = calloc(dlen, sizeof(char));
	if (!d) {
		fprintf(stderr, "[!] Erreur d’allocation mémoire.");
	}
	d[0] = '<';
	memcpy(d + 1, data, nlen);
	d[1 + nlen] = '>';
	d[1 + nlen + 1] = ' ';
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(g_text_view_messages_buffer), NULL, "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1, tag, NULL);

	dlen = len + 1 + 1 - nlen - 1;
	free(d);
	d = calloc(dlen, sizeof(char));
	if (!d) {
		fprintf(stderr, "[!] Erreur d’allocation mémoire.");
	}
	snprintf(d, dlen, "%s\n", data + nlen + 1);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(g_text_view_messages), &end, 0.1, FALSE, 0, 0);

	update_title(12);
}

void send_message() {
	const char *s = gtk_entry_get_text(GTK_ENTRY(g_entry_message));
	// const char *s = "nickk:Hello la famille";
	print_message (s, strlen(s), 0);
	gtk_entry_set_text(GTK_ENTRY(g_entry_message), "");
	printf("%s\n", s);
}

int main2(void) {
  // Initialisation du générateur de nombres aléatoires.
  srand((unsigned)time(NULL));
  new_client_id();

  //set_server_port((uint16_t)atoi(argv[1]));
  set_server_port(4242);

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

  printf("Thread_creation...\n");

  pthread_create(&srv, NULL, udp_server, NULL);
  //pthread_create(&thread_id[1], NULL, list_loop, (void*)&thread_id[1]);
  //pthread_create(&thread_id[2], NULL, listen_input, (void*)&thread_id[2]);

  // pthread_join(thread_id[2], NULL);

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
