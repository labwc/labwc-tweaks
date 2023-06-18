// SPDX-License-Identifier: GPL-2.0-only
#include "keyboard-layouts.h"
#include "state.h"
#include "stack-mouse.h"
#include "theme.h"
#include "update.h"
#include "xml.h"

void
stack_mouse_init(struct state *state, GtkWidget *stack)
{
	GtkWidget *widget;

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_stack_add_named(GTK_STACK(stack), vbox, "mouse");
	gtk_container_child_set(GTK_CONTAINER(stack), vbox, "title", "Mouse and Touchpad", NULL);

	/* the grid with settings */
	int row = 0;
	GtkWidget *grid = gtk_grid_new();
	g_object_set(grid, "margin", 20, "row-spacing", 10, "column-spacing", 10, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);

	/* cursor theme combobox */
	struct themes cursor_themes = { 0 };
	theme_find(&cursor_themes, "icons", "cursors");

	widget = gtk_label_new("cursor theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.cursor_theme_name = gtk_combo_box_text_new();

	char *active_id = g_settings_get_string(state->settings, "cursor-theme");
	int active = -1;
	for (int i = 0; i < cursor_themes.nr; ++i) {
		struct theme *theme = cursor_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.cursor_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.cursor_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), state->widgets.cursor_theme_name, 1, row++, 1, 1);
	theme_free_vector(&cursor_themes);

	/* cursor size spinbutton */
	widget = gtk_label_new("cursor size");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	GtkAdjustment *cursor_adjustment = gtk_adjustment_new(0, 0, 512, 1, 2, 0);
	state->widgets.cursor_size = gtk_spin_button_new(GTK_ADJUSTMENT(cursor_adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->widgets.cursor_size), g_settings_get_int(state->settings, "cursor-size"));
	gtk_grid_attach(GTK_GRID(grid), state->widgets.cursor_size, 1, row++, 1, 1);

	/* natural scroll combobox */
	widget = gtk_label_new("natural scroll");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.natural_scroll = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.natural_scroll), "no");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.natural_scroll), "yes");
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.natural_scroll), xml_get_bool_text("/labwc_config/libinput/device/naturalscroll"));
	gtk_grid_attach(GTK_GRID(grid), state->widgets.natural_scroll, 1, row++, 1, 1);

	/* bottom button box */
	GtkWidget *hbbox = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(vbox), hbbox, TRUE, TRUE, 5);
	widget = gtk_button_new_with_label("Update");
	g_signal_connect(widget, "clicked", G_CALLBACK(update), state);
	gtk_container_add(GTK_CONTAINER(hbbox), widget);
	widget = gtk_button_new_with_label("Quit");
	g_signal_connect_swapped(widget, "clicked", G_CALLBACK(gtk_widget_destroy), state->window);
	gtk_container_add(GTK_CONTAINER(hbbox), widget);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(hbbox), GTK_BUTTONBOX_END);
}

