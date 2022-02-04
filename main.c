#define _POSIX_C_SOURCE 200809L
#include <gtk/gtk.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include "tweaks.h"

static GtkWidget *corner_radius;
static GtkWidget *openbox_theme_name;
static GtkWidget *gtk_theme_name;
static GtkWidget *icon_theme_name;
static GtkWidget *cursor_theme_name;
static GtkWidget *natural_scroll;

static struct themes openbox_themes = { 0 };
static struct themes gtk_themes = { 0 };

static void
update(GtkWidget *widget, gpointer data)
{
	/* corner radius, labwc theme, libinput scroll */
	xml_set_num("cornerradius.theme", gtk_spin_button_get_value(GTK_SPIN_BUTTON(corner_radius)));
	xml_set("name.theme", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(openbox_theme_name)));
	xml_set("naturalscroll.device.libinput", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(natural_scroll)));
	xml_save();

	/* set cursor for gtk */
	char buf_cur[4096];
	snprintf(buf_cur, sizeof(buf_cur), "gsettings set org.gnome.desktop.interface cursor-theme %s",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cursor_theme_name)));
	popen(buf_cur, "r");
	
	/* set cursor for labwc  - should cover 'replace' or 'append' */
	char xcur[15] = "XCURSOR_THEME=";
	char filename[PATH_MAX];
	char bufname[PATH_MAX];
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
	fprintf(fw, "%s", strcat(xcur,
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cursor_theme_name))));
	fclose(fw);
	rename(bufname, filename);	
	
	/* reset labwc */
	if (!fork()) {
		execl("/bin/sh", "/bin/sh", "-c", "killall -SIGHUP labwc", (void *)NULL);
	}

	/* gtk theme */
	char buf[4096];
	snprintf(buf, sizeof(buf), "gsettings set org.gnome.desktop.interface gtk-theme %s",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(gtk_theme_name)));
	popen(buf, "r");

	/* some icon themes have a space in the name */
	char buf_ico[4096];
	snprintf(buf_ico, sizeof(buf_ico), "gsettings set org.gnome.desktop.interface icon-theme \"%s\"",
		gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(icon_theme_name)));
	popen(buf_ico, "r");
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

	/* handle <foo></foo> and <foo /> where no value has been specified */
	if (!value) {
		return -1;
	}

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
	pclose(fp);
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

	/* icon theme combobox */
	widget = gtk_label_new("icon theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	icon_theme_name = gtk_combo_box_text_new();

	char path0[PATH_MAX];
	struct themes icon_themes = { 0 };
	snprintf(path0, sizeof(path0), "%s/%s", home, ".local/share/icons");
	find_themes(&icon_themes, path0, "scalable");
	find_themes(&icon_themes, "/usr/share/icons", "scalable");
	qsort(icon_themes.data, icon_themes.nr, sizeof(struct theme), compare);

	active = -1;
	FILE *fp0 = popen("gsettings get org.gnome.desktop.interface icon-theme", "r");
	char buf0[4096] = { 0 };
	fgets(buf0, sizeof(buf0), fp);
	char *p0 = strrchr(buf0, '\n');
	if (p0) {
		*p0 = '\0';
	}
	fclose(fp0);
	active_id = remove_single_quotes(buf0);

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

	struct themes cursor_themes = { 0 };
	find_themes(&cursor_themes, path0, "cursors/xterm");
	find_themes(&cursor_themes, "/usr/share/icons", "cursors/xterm");
	qsort(cursor_themes.data, cursor_themes.nr, sizeof(struct theme), compare);

	active = -1;
	FILE *fp1 = popen("gsettings get org.gnome.desktop.interface cursor-theme", "r");
	char buf1[4096] = { 0 };
	fgets(buf1, sizeof(buf1), fp);
	char *p1 = strrchr(buf1, '\n');
	if (p1) {
		*p1 = '\0';
	}
	fclose(fp1);
	active_id = remove_single_quotes(buf1);
	
	for (int i = 0; i < cursor_themes.nr; ++i) {
		theme = cursor_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(cursor_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(cursor_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid),cursor_theme_name, 1, row++, 1, 1);

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

	/* clean up */
	xml_finish();
	free_theme_vector(&openbox_themes);
	free_theme_vector(&gtk_themes);
	pango_cairo_font_map_set_default(NULL);

	return status;
}
