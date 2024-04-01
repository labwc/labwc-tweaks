// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include "state.h"
#include "stack-appearance.h"
#include "stack-lang.h"
#include "stack-mouse.h"
#include "update.h"
#include "xml.h"

static void
activate(GtkApplication *app, gpointer user_data)
{
	struct state *state = (struct state *)user_data;

	/* window */
	state->window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(state->window), "Tweaks");

	/* grid */
	GtkWidget *grid = gtk_grid_new();
	gtk_container_add(GTK_CONTAINER(state->window), grid);
	GtkWidget *sidebar = gtk_stack_sidebar_new();
	GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	GtkWidget *stack = gtk_stack_new();
	GtkWidget *bottom_buttons = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_grid_attach(GTK_GRID(grid), sidebar, 0, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(grid), separator, 1, 0, 1, 2);
	gtk_grid_attach(GTK_GRID(grid), stack, 2, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bottom_buttons, 0, 1, 3, 1);

	/* sidebar + stack */
	gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(stack));
	stack_appearance_init(state, stack);
	stack_mouse_init(state, stack);
	stack_lang_init(state, stack);

	/* bottom buttons */
	GtkWidget *button = gtk_button_new_with_label("Update");
	g_signal_connect(button, "clicked", G_CALLBACK(update), state);
	gtk_container_add(GTK_CONTAINER(bottom_buttons), button);
	button = gtk_button_new_with_label("Quit");
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_widget_destroy), state->window);
	gtk_container_add(GTK_CONTAINER(bottom_buttons), button);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(bottom_buttons), GTK_BUTTONBOX_END);

	/* show */
	gtk_widget_show_all(state->window);
}

int
main(int argc, char **argv)
{
	struct state state = { 0 };

	/* read/create config file */
	char filename[4096];
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/rc.xml");
	xml_init(filename);

	/* connect to gsettings */
	state.settings = g_settings_new("org.gnome.desktop.interface");

	/* start ui */
	GtkApplication *app;
	int status;
	app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(activate), &state);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	/* clean up */
	xml_finish();
	pango_cairo_font_map_set_default(NULL);

	return status;
}
