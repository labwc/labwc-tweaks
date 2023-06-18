// SPDX-License-Identifier: GPL-2.0-only
#include "keyboard-layouts.h"
#include "state.h"
#include "stack-appearance.h"
#include "theme.h"
#include "update.h"
#include "xml.h"

void
stack_appearance_init(struct state *state, GtkWidget *stack)
{
	GtkWidget *widget;

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_stack_add_named(GTK_STACK(stack), vbox, "appearance");
	gtk_container_child_set(GTK_CONTAINER(stack), vbox, "title", "Appearance", NULL);

	/* the grid with settings */
	int row = 0;
	GtkWidget *grid = gtk_grid_new();
	g_object_set(grid, "margin", 20, "row-spacing", 10, "column-spacing", 10, NULL);
	gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 5);

	/* openbox theme combobox */
	struct themes openbox_themes = { 0 };
	theme_find(&openbox_themes, "themes", "openbox-3/themerc");
	widget = gtk_label_new("openbox theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.openbox_theme_name = gtk_combo_box_text_new();
	int active = -1;
	char *active_id = xml_get("/labwc_config/theme/name");
	struct theme *theme;
	for (int i = 0; i < openbox_themes.nr; ++i) {
		theme = openbox_themes.data + i;
		if (active_id && !strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.openbox_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.openbox_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), state->widgets.openbox_theme_name, 1, row++, 1, 1);
	theme_free_vector(&openbox_themes);

	/* corner radius spinbutton */
	widget = gtk_label_new("corner radius");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	GtkAdjustment *adjustment = gtk_adjustment_new(0, 0, 10, 1, 2, 0);
	state->widgets.corner_radius = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(state->widgets.corner_radius), xml_get_int("/labwc_config/theme/cornerradius"));
	gtk_grid_attach(GTK_GRID(grid), state->widgets.corner_radius, 1, row++, 1, 1);

	/* gtk theme combobox */
	struct themes gtk_themes = { 0 };
	theme_find(&gtk_themes, "themes", "gtk-3.0/gtk.css");

	widget = gtk_label_new("gtk theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.gtk_theme_name = gtk_combo_box_text_new();

	active_id = g_settings_get_string(state->settings, "gtk-theme");
	active = -1;
	for (int i = 0; i < gtk_themes.nr; ++i) {
		theme = gtk_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.gtk_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.gtk_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), state->widgets.gtk_theme_name, 1, row++, 1, 1);
	theme_free_vector(&gtk_themes);

	/* icon theme combobox */
	struct themes icon_themes = { 0 };
	theme_find(&icon_themes, "icons", NULL);

	widget = gtk_label_new("icon theme");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.icon_theme_name = gtk_combo_box_text_new();

	active_id = g_settings_get_string(state->settings, "icon-theme");
	active = -1;
	for (int i = 0; i < icon_themes.nr; ++i) {
		theme = icon_themes.data + i;
		if (!strcmp(theme->name, active_id)) {
			active = i;
		}
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(state->widgets.icon_theme_name), theme->name);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(state->widgets.icon_theme_name), active);
	gtk_grid_attach(GTK_GRID(grid), state->widgets.icon_theme_name, 1, row++, 1, 1);
	theme_free_vector(&icon_themes);

	/* keyboard layout */
	GList *keyboard_layouts = NULL;
	keyboard_layouts_init(&keyboard_layouts, "/usr/share/X11/xkb/rules/evdev.lst");

	widget = gtk_label_new("keyboard layout");
	gtk_widget_set_halign(widget, GTK_ALIGN_START);
	gtk_grid_attach(GTK_GRID(grid), widget, 0, row, 1, 1);
	state->widgets.keyboard_layout = gtk_combo_box_text_new();

	active_id = "gb";
	active = -1;

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

