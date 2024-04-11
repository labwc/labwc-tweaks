/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef STATE_H
#define STATE_H
#include <gtk/gtk.h>

struct state {
	GtkWidget *window;
	struct {
		GtkWidget *corner_radius;
		GtkWidget *openbox_theme_name;
		GtkWidget *gtk_theme_name;
		GtkWidget *icon_theme_name;
		GtkWidget *cursor_theme_name;
		GtkWidget *cursor_size;
		GtkWidget *natural_scroll;
		GtkWidget *keyboard_layout;
	} widgets;
	GSettings *settings;
};

#endif /* STATE_H */
