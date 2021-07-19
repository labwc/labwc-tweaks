#define _POSIX_C_SOURCE 200809L
#include <gtk/gtk.h>
#include <string.h>
#include "tweaks.h"

static GtkWidget *corner_radius;
static GtkWidget *theme_name;

static void
update(GtkWidget *widget, gpointer data)
{
	xml_set_num("cornerradius.theme", gtk_spin_button_get_value(GTK_SPIN_BUTTON(corner_radius)));
	xml_set("name.theme", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(theme_name)));
	xml_save();
	if (!fork()) {
		execl("/bin/sh", "/bin/sh", "-c", "killall -SIGHUP labwc", (void *)NULL);
	}
}

static void
activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window, *vbox, *grid, *widget, *hbbox;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "labwc-tweaks");
	gtk_window_set_default_size(GTK_WINDOW(window), -1, -1);

	/* root container */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* the grid with settings */
	int row = 0;
	grid = gtk_grid_new();
	g_object_set(grid, "margin", 20, "row-spacing", 10, "column-spacing", 10, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);

	/* theme combobox */
	widget = gtk_label_new("theme");
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	theme_name = gtk_combo_box_text_new();

	char filename[PATH_MAX];
	struct themes themes = { 0 };
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".local/share/themes");
	find_themes(&themes, filename);
	find_themes(&themes, "/usr/share/themes");
	struct theme *theme;
	for (int i = 0; i < themes.nr; ++i) {
		theme = themes.data + i;
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(theme_name), theme->name);
	}

	gtk_combo_box_set_active(GTK_COMBO_BOX(theme_name), 0);
	gtk_grid_attach(GTK_GRID(grid), theme_name, 1, row++, 1, 1);

	/* corner radius spinbutton */
	widget = gtk_label_new("corner radius");
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 2, 0);
	corner_radius = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(corner_radius), xml_get_int("cornerradius.theme"));
	gtk_grid_attach(GTK_GRID(grid), corner_radius, 1, row++, 1, 1);

	/* bottom button box */
	hbbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(vbox), hbbox, TRUE, TRUE, 5);
	widget = gtk_button_new_with_label("Update");
	g_signal_connect(widget, "clicked", G_CALLBACK(update), NULL);
	gtk_container_add(GTK_CONTAINER(hbbox), widget);
	widget = gtk_button_new_with_label("Quit");
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(gtk_widget_destroy), window);
	gtk_container_add(GTK_CONTAINER(hbbox), widget);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_END);

	gtk_widget_show_all(window);
}

int
main(int argc, char **argv)
{
	/* read config file */
	char filename[PATH_MAX];
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/rc.xml");

	xml_init(filename);

	/* start ui */
	GtkApplication *app;
	int status;
	app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	xml_finish();

	return status;
}
