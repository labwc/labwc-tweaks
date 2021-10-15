#define _POSIX_C_SOURCE 200809L
#include <gtk/gtk.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include "tweaks.h"

static GtkWidget *corner_radius;
static GtkWidget *openbox_theme_name;
static GtkWidget *gtk_theme_name;
static GtkWidget *natural_scroll;

static void
update(GtkWidget *widget, gpointer data)
{
	xml_set_num("cornerradius.theme", gtk_spin_button_get_value(GTK_SPIN_BUTTON(corner_radius)));
	xml_set("name.theme", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(openbox_theme_name)));
	xml_set("naturalscroll.device.libinput", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(natural_scroll)));
	xml_save();
	if (!fork()) {
		execl("/bin/sh", "/bin/sh", "-c", "killall -SIGHUP labwc", (void *)NULL);
	}

	char buf[4096];
	snprintf(buf, sizeof(buf), "gsettings set org.gnome.desktop.interface gtk-theme %s",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gtk_theme_name)));
	popen(buf, "r");
}

/* Sort system themes in alphabetical order */
static int
compare(const void *a, const void *b)
{
	const struct theme *theme_a = (struct theme *)a;
	const struct theme *theme_b = (struct theme *)b;
	return strcasecmp(theme_a->name, theme_b->name);
}

static char *
remove_single_quotes(char *buf)
{
	char *s;
	s = buf;
	if (s[0] == '\'') {
		++s;
	}
	char *p = strrchr(s, '\'');
	if (p) {
		*p = '\0';
	}
	return s;
}

static int
get_yes_no(char *nodename)
{
	char *value = xml_get(nodename);
	if (!strcmp(value, "yes")) {
		return 0;
	} else if (!strcmp(value, "no")) {
		return 1;
	} else {
		return -1;
	}
}

static void
activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window, *vbox, *grid, *widget, *hbbox;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Tweaks");
	gtk_window_set_default_size(GTK_WINDOW(window), -1, -1);

	/* root container */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);

	/* the grid with settings */
	int row = 0;
	grid = gtk_grid_new();
	g_object_set(grid, "margin", 20, "row-spacing", 10, "column-spacing", 10, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);

	/* openbox theme combobox */
	widget = gtk_label_new("openbox theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	openbox_theme_name = gtk_combo_box_text_new();

	char path[4096];
	struct themes openbox_themes = { 0 };
	char *home = getenv("HOME");
	snprintf(path, sizeof(path), "%s/%s", home, ".local/share/themes");
	find_themes(&openbox_themes, path, "openbox-3/themerc");
	find_themes(&openbox_themes, "/usr/share/themes", "openbox-3/themerc");
	qsort(openbox_themes.data, openbox_themes.nr, sizeof(struct theme), compare);

	int active = -1;
	char *active_id = xml_get("name.theme");
	struct theme *theme;
	for (int i = 0; i < openbox_themes.nr; ++i) {
		theme = openbox_themes.data + i;
		if (active_id && !strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(openbox_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(openbox_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), openbox_theme_name, 1, row++, 1, 1);

	/* corner radius spinbutton */
	widget = gtk_label_new("corner radius");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 2, 0);
	corner_radius = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(corner_radius), xml_get_int("cornerradius.theme"));
	gtk_grid_attach(GTK_GRID(grid), corner_radius, 1, row++, 1, 1);

	/* gtk theme combobox */
	widget = gtk_label_new("gtk theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	gtk_theme_name = gtk_combo_box_text_new();

	struct themes gtk_themes = { 0 };
	find_themes(&gtk_themes, path, "gtk-3.0/gtk.css");
	find_themes(&gtk_themes, "/usr/share/themes", "gtk-3.0/gtk.css");
	qsort(gtk_themes.data, gtk_themes.nr, sizeof(struct theme), compare);

	active = -1;
	FILE *fp = popen("gsettings get org.gnome.desktop.interface gtk-theme", "r");
	char buf[4096] = { 0 };
	fgets(buf, sizeof(buf), fp);
	char *p = strrchr(buf, '\n');
	if (p) {
		*p = '\0';
	}
	fclose(fp);
	active_id = remove_single_quotes(buf);

	for (int i = 0; i < gtk_themes.nr; ++i) {
		theme = gtk_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), gtk_theme_name, 1, row++, 1, 1);

	/* natural scroll combobox */
	widget = gtk_label_new("natural scroll");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	natural_scroll = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(natural_scroll), "yes");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(natural_scroll), "no");
	gtk_combo_box_set_active(GTK_COMBO_BOX(natural_scroll), get_yes_no("naturalscroll.device.libinput"));
	gtk_grid_attach(GTK_GRID(grid), natural_scroll, 1, row++, 1, 1);

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

	struct stat st;
	if (stat(filename, &st)) {
		printf("error: need ~/.config/labwc/rc.xml to run\n");
		exit(EXIT_FAILURE);
	}

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
