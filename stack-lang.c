// SPDX-License-Identifier: GPL-2.0-only
#include "keyboard-layouts.h"
#include "state.h"
#include "stack-lang.h"
#include "theme.h"
#include "xml.h"

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

	char *active_id = "gb";
	int active = -1;

	GList *iter;
	int i = 0;
	for (iter = keyboard_layouts; iter; iter = iter->next) {
		struct layout *layout = (struct layout *)iter->data;
		if (!strcmp(layout->lang, active_id)) {
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

