#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <gio/gio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include "tweaks.h"
#include "xml.h"

static GtkWidget *corner_radius;
static GtkWidget *openbox_theme_name;
static GtkWidget *gtk_theme_name;
static GtkWidget *icon_theme_name;
static GtkWidget *cursor_theme_name;
static GtkWidget *cursor_size;
static GtkWidget *natural_scroll;

static struct themes openbox_themes = { 0 };
static struct themes gtk_themes = { 0 };
static struct themes icon_themes = { 0 };
static struct themes cursor_themes = { 0 };

static GSettings *settings;

void
spawn_sync(char const *command)
{
	GError *err = NULL;
	assert(command);
	g_spawn_command_line_sync(command, NULL, NULL, NULL, &err);
	if (err) {
		fprintf(stderr, "warn: could not find %s\n", command);
		g_error_free(err);
	}
}


static void
environment_set(const char *key, const char *value)
{
	/* set cursor for labwc  - should cover 'replace' or 'append' */
	char xcur[4096] = {0};
	strcpy(xcur, key);
	strcat(xcur, "=");
	char filename[4096];
	char bufname[4096];
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/environment");
	snprintf(bufname, sizeof(bufname), "%s/%s", home, ".config/labwc/buf");
	FILE *fe = fopen(filename, "r");
	FILE *fw = fopen(bufname, "a");
	if((fe == NULL) || (fw == NULL)) {
		perror("Unable to open file!");
		return;
	}
	char chunk[128];
	while(fgets(chunk, sizeof(chunk), fe) != NULL) {
		if (strstr(chunk, xcur) != NULL) {
			continue;
		} else {
			fprintf(fw, "%s", chunk);
		}
	}
	fclose(fe);
	if (value) {
		fprintf(fw, "%s\n", strcat(xcur, value));
	}
	fclose(fw);
	rename(bufname, filename);	
}

static void
environment_set_num(const char *key, int value) {
	char buffer[255];
	snprintf(buffer, 255, "%d", value);

	environment_set(key, buffer);
}

static void
set_value(GSettings *settings, const char *key, const char *value)
{
	if (!value) {
		fprintf(stderr, "warn: cannot set '%s' - no value specified\n", key);
		return;
	}
	g_settings_set_value(settings, key, g_variant_new("s", value));
}

static void
set_value_num(GSettings *settings, const char *key, int value)
{
	g_settings_set_value(settings, key, g_variant_new("i", value));
}

#define COMBO_TEXT(w) gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w))
#define SPIN_BUTTON_VAL(w) gtk_spin_button_get_value(GTK_SPIN_BUTTON(w))
#define SPIN_BUTTON_VAL_INT(w) (int)SPIN_BUTTON_VAL(w)

static void
update(GtkWidget *widget, gpointer data)
{
	/* ~/.config/labwc/rc.xml */
	xml_set_num("/labwc_config/theme/cornerradius", SPIN_BUTTON_VAL(corner_radius));
	xml_set("/labwc_config/theme/name", COMBO_TEXT(openbox_theme_name));
	xml_set("/labwc_config/libinput/device/naturalscroll", COMBO_TEXT(natural_scroll));
	xml_save();

	/* gsettings */
	set_value(settings, "cursor-theme", COMBO_TEXT(cursor_theme_name));
	set_value_num(settings, "cursor-size", SPIN_BUTTON_VAL_INT(cursor_size));
	set_value(settings, "gtk-theme", COMBO_TEXT(gtk_theme_name));
	set_value(settings, "icon-theme", COMBO_TEXT(icon_theme_name));
	
	/* ~/.config/labwc/environment */
	environment_set("XCURSOR_THEME", COMBO_TEXT(cursor_theme_name));
	environment_set_num("XCURSOR_SIZE", SPIN_BUTTON_VAL_INT(cursor_size));

	if (!g_strcmp0(COMBO_TEXT(openbox_theme_name), "GTK")) {
		spawn_sync("labwc-gtktheme.py");
	}

	/* reconfigure labwc */
	if (!fork()) {
		execl("/bin/sh", "/bin/sh", "-c", "killall -s SIGHUP labwc", (void *)NULL);
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
	int active = -1;
	char *active_id = xml_get("/labwc_config/theme/name");
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
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(corner_radius), xml_get_int("/labwc_config/theme/cornerradius"));
	gtk_grid_attach(GTK_GRID(grid), corner_radius, 1, row++, 1, 1);

	/* gtk theme combobox */
	widget = gtk_label_new("gtk theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	gtk_theme_name = gtk_combo_box_text_new();

	active_id = g_settings_get_string(settings, "gtk-theme");
	active = -1;
	for (int i = 0; i < gtk_themes.nr; ++i) {
		theme = gtk_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(gtk_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), gtk_theme_name, 1, row++, 1, 1);

	/* icon theme combobox */
	widget = gtk_label_new("icon theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	icon_theme_name = gtk_combo_box_text_new();

	active_id = g_settings_get_string(settings, "icon-theme");
	active = -1;
	for (int i = 0; i < icon_themes.nr; ++i) {
		theme = icon_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(icon_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(icon_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid),icon_theme_name, 1, row++, 1, 1);

	/* cursor theme combobox */
	widget = gtk_label_new("cursor theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	cursor_theme_name = gtk_combo_box_text_new();

	active_id = g_settings_get_string(settings, "cursor-theme");
	active = -1;
	for (int i = 0; i < cursor_themes.nr; ++i) {
		theme = cursor_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cursor_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(cursor_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid),cursor_theme_name, 1, row++, 1, 1);

	/* cursor size spinbutton */
	widget = gtk_label_new("cursor size");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	GtkAdjustment *cursor_adjustment = gtk_adjustment_new(0, 0, 512, 1, 2, 0);
	cursor_size = gtk_spin_button_new(GTK_ADJUSTMENT(cursor_adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(cursor_size), g_settings_get_int(settings, "cursor-size"));
	gtk_grid_attach(GTK_GRID(grid), cursor_size, 1, row++, 1, 1);

	/* natural scroll combobox */
	widget = gtk_label_new("natural scroll");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	natural_scroll = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(natural_scroll), "no");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(natural_scroll), "yes");
	gtk_combo_box_set_active(GTK_COMBO_BOX(natural_scroll), xml_get_bool_text("/labwc_config/libinput/device/naturalscroll"));
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

static void
free_theme_vector(struct themes *themes)
{
	for (int i = 0; i < themes->nr; ++i) {
		struct theme *theme = themes->data + i;
		if (theme->name) {
			free(theme->name);
		}
		if (theme->path) {
			free(theme->path);
		}
	}
	free(themes->data);
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

	/* load themes */
	find_themes(&openbox_themes, "themes", "openbox-3/themerc");
	find_themes(&gtk_themes, "themes", "gtk-3.0/gtk.css");
	find_themes(&cursor_themes, "icons", "cursors");
	find_themes(&icon_themes, "icons", NULL);

	/* connect to gsettings */
	settings = g_settings_new("org.gnome.desktop.interface");

	/* start ui */
	GtkApplication *app;
	int status;
	app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	/* clean up */
	xml_finish();
	free_theme_vector(&openbox_themes);
	free_theme_vector(&gtk_themes);
	free_theme_vector(&icon_themes);
	free_theme_vector(&cursor_themes);
	pango_cairo_font_map_set_default(NULL);

	return status;
}
