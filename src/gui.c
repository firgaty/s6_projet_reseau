#include "types.h"
#include "gui.h"

void *init_gui() {
// void *init_gui(int argc, char *argv[]) {
  GtkBuilder *builder;

  gtk_init(0, NULL);
  // gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "glade/window_main.glade", NULL);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window_main"));
  gtk_builder_connect_signals(builder, NULL);
  g_label_infos2 = GTK_LABEL(gtk_builder_get_object(builder, "label_infos2"));
  g_label_peers = GTK_LABEL(gtk_builder_get_object(builder, "label_peers"));
  g_text_view_messages = GTK_WIDGET(gtk_builder_get_object(builder, "text_view_messages"));
  g_text_view_messages_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "text_view_messages_buffer"));
  g_entry_message = GTK_WIDGET(gtk_builder_get_object(builder, "entry_message"));
  g_button_send = GTK_WIDGET(gtk_builder_get_object(builder, "button_send"));

  print_info("Bienvenue ;-)");

  g_object_unref(builder);

  gtk_widget_show(window);
  gtk_main();
	return 0;
}

void on_window_main_destroy()
{
    gtk_main_quit();
}

void on_entry_message_activate() {
	send_message();
}

void on_button_send_clicked() {
	send_message();
}

size_t nick_len(const char *data, size_t data_len) {
	size_t nlen;

	nlen = 0;
	while (*data && nlen <= data_len && *data != ':') {
		nlen++;
		data++;
	}
	return nlen == data_len ? 0 : nlen;
}

// nsym: nombre de voisins symétriques
void update_title(size_t nsym) {
	char title[256];

	memset(title, 0, 256);
	snprintf(title, 256, "[%ld] miaouchat", nsym);
	gtk_window_set_title(GTK_WINDOW(window), title);
}

void update_label_infos2(size_t nsym) {
	char label[64];

	memset(label, 0, 64);
	snprintf(label, 64, "Pairs : %ld", nsym);
	gtk_label_set_text(GTK_LABEL(g_label_infos2), label);
}

void update_label_peers(size_t nsym) {
	char label[64];

	memset(label, 0, 64);
	snprintf(label, 64, "Pairs : %ld", nsym);
	gtk_label_set_text(GTK_LABEL(g_label_peers), label);
}

// char *get_text() {
// 	GtkTextIter start, end;
// 	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view_messages));
// 	gchar *text;
// 	gtk_get_buffer_get_bounds(buffer, &start, &end);
// 	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
// 	return text;
// }

void print_info(const char *message) {
	time_t rt;
	struct tm *t;
	size_t dlen;
	char *d;
	GtkTextIter end;
	GtkTextTag *tag;

	time(&rt);
	t = localtime(&rt);

	dlen = 10 + 1 + 1;
	d = calloc(dlen, sizeof(char));
	if (!d) {
		fprintf(stderr, "[!] Erreur d’allocation mémoire.");
	}
	strftime(d, dlen, "[%H:%M:%S] ", t);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(g_text_view_messages_buffer), NULL, "weight", PANGO_WEIGHT_LIGHT, NULL);
	gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1, tag, NULL);

	free(d);
	dlen = 6 + 1 + strlen(message) + 1 + 1;
	d = calloc(dlen, sizeof(char));
	snprintf(d, dlen, "<INFO> %s\n", message);
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	tag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(g_text_view_messages_buffer), NULL, "foreground", "orange", NULL);
	gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1, tag, NULL);
}

void print_message(const char *data, size_t len) {
	time_t rt;
	struct tm *t;
	size_t dlen;
	size_t nlen;
	char *d;
	GtkTextIter end;
	GtkTextTag *tag;

	if (HIDE_EMPTY_MESSAGES && len == 0 || (len == 1 && data[0] == '\0')) {
		return;
	}

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

	// dlen = len + 1 + 1 - nlen - 1;
	dlen = len + 1 + 1 - nlen;
	free(d);
	d = calloc(dlen, sizeof(char));
	if (!d) {
		fprintf(stderr, "[!] Erreur d’allocation mémoire.");
	}
	snprintf(d, dlen, "%s\n", data + nlen + (nlen == 0 ? 0 : 1));
	gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(g_text_view_messages_buffer), &end, d, -1);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(g_text_view_messages), &end, 0.1, FALSE, 0, 0);
}

void send_message() {
	const char *s = gtk_entry_get_text(GTK_ENTRY(g_entry_message));
	// const char *s = "nickk:Hello la famille";
	print_message(s, strlen(s));
	// print_info("Information spéciale, coucou tout le monde.");
	update_label_peers(4);
	gtk_entry_set_text(GTK_ENTRY(g_entry_message), "");
	printf("%s\n", s);
}
