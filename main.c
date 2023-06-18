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
#include "xml.h"

static void
activate(GtkApplication *app, gpointer user_data)
{
	struct state *state = (struct state *)user_data;

	/* window */
	state->window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(state->window), "Tweaks");
	gtk_window_set_default_size(GTK_WINDOW(state->window), 640, 480);

	/* grid */
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
	gtk_container_add(GTK_CONTAINER(state->window), grid);

	/* sidebar + stack */
	GtkWidget *sidebar = gtk_stack_sidebar_new();
	GtkWidget *stack = gtk_stack_new();
	GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
	gtk_grid_attach(GTK_GRID(grid), sidebar, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), separator, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), stack, 2, 0, 1, 1);
	gtk_stack_sidebar_set_stack(GTK_STACK_SIDEBAR(sidebar), GTK_STACK(stack));

	/* Tabs */
	stack_appearance_init(state, stack);

	GtkWidget *bar = gtk_label_new("Mouse & Touchpad");
	gtk_stack_add_named(GTK_STACK(stack), bar, "mouse_and_touchpad");
	gtk_container_child_set(GTK_CONTAINER(stack), bar, "title", "Mouse & Touchpad", NULL);

	GtkWidget *baz = gtk_label_new("Language & Region");
	gtk_stack_add_named(GTK_STACK(stack), baz, "language_and_region");
	gtk_container_child_set(GTK_CONTAINER(stack), baz, "title", "Language & Region", NULL);

	gtk_widget_show_all(state->window);
}

static const char rcxml_template[] =
	"<?xml version=\"1.0\"?>\n"
	"<labwc_config>\n"
	"  <core>\n"
	"  </core>\n"
	"</labwc_config>\n";

static void
create_basic_rcxml(const char *filename)
{
	FILE *file = fopen(filename, "w");
	if (!file) {
		fprintf(stderr, "warn: fopen(%s) failed\n", filename);
		return;
	}
	if (!fwrite(rcxml_template, sizeof(rcxml_template), 1, file)) {
		fprintf(stderr, "warn: error writing to %s", filename);
	}
	fclose(file);
}
int
main(int argc, char **argv)
{
	struct state state = { 0 };

	/* read/create config file */
	char filename[4096];
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/rc.xml");
	if (access(filename, F_OK)) {
		create_basic_rcxml(filename);
	}
	xml_init(filename);

	/* ensure all relevant nodes exist before we start getting/setting */
	xpath_add_node("/labwc_config/theme/cornerRadius");
	xpath_add_node("/labwc_config/theme/name");
	xpath_add_node("/labwc_config/libinput/device/naturalScroll");
	xml_save();

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
