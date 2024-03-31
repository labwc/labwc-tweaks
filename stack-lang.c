// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include "keyboard-layouts.h"
#include "state.h"
#include "stack-lang.h"
#include "theme.h"
#include "xml.h"

static void
rtrim(char **s)
{
	size_t len = strlen(*s);
	if (!len) {
		return;
	}
	char *end = *s + len - 1;
	while (end >= *s && isspace(*end)) {
		end--;
	}
	*(end + 1) = '\0';
}

static char *
string_strip(char *s)
{
	rtrim(&s);
	while (isspace(*s)) {
		s++;
	}
	return s;
}

static char *
get_value(char *line, const char *key)
{
	if (!line || !*line || line[0] == '#') {
		return NULL;
	}
	char *p = strchr(line, '=');
	if (!p) {
		return NULL;
	}
	*p = '\0';
	if (!!strcmp(key, string_strip(line))) {
		return NULL;
	}
	char *value = string_strip(++p);
	return value ? value : NULL;
}

static void
environment_get(char *buffer, size_t size, const char *key)
{
	char filename[4096];
	snprintf(filename, sizeof(filename), "%s/%s", getenv("HOME"), ".config/labwc/environment");

	char *value = NULL;
	char *line = NULL;
	size_t len = 0;
	FILE *stream = fopen(filename, "r");
	if (!stream) {
		return;
	}

	while (getline(&line, &len, stream) != -1) {
		char *p = strrchr(line, '\n');
		if (p) {
			*p = '\0';
		}
		value = get_value(line, key);
		if (value) {
			snprintf(buffer, size, "%s", value);
			break;
		}
	}
	free(line);
	fclose(stream);
}

void
stack_lang_init(struct state *state, GtkWidget *stack)
{
	GtkWidget *widget;

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_stack_add_named(GTK_STACK(stack), vbox, "lang");
	gtk_container_child_set(GTK_CONTAINER(stack), vbox, "title", "Language & Region", NULL);

	/* the grid with settings */
	int row = 0;
	GtkWidget *grid = gtk_grid_new();
	g_object_set(grid, "margin", 20, "row-spacing", 10, "column-spacing", 10, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);

	/* keyboard layout */
	GList *keyboard_layouts = NULL;
	keyboard_layouts_init(&keyboard_layouts, "/usr/share/X11/xkb/rules/evdev.lst");

	widget = gtk_label_new("keyboard layout");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.keyboard_layout = gtk_combo_box_text_new();

	char xkb_default_layout[1024];
	environment_get(xkb_default_layout, sizeof(xkb_default_layout), "XKB_DEFAULT_LAYOUT");
	int active = -1;

	GList *iter;
	int i = 0;
	for (iter = keyboard_layouts; iter; iter = iter->next) {
		struct layout *layout = (struct layout *)iter->data;
		if (!strcmp(layout->lang, xkb_default_layout)) {
			active = i;
		}
		char buf[256];
		snprintf(buf, sizeof(buf), "%s  %s", layout->lang, layout->description);
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.keyboard_layout), buf);
		++i;
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.keyboard_layout), active);
	gtk_grid_attach(GTK_GRID(grid), state->widgets.keyboard_layout, 1, row++, 1, 1);
	keyboard_layouts_finish(keyboard_layouts);
}

