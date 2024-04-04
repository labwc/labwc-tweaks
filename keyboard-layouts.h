/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef KEYBOARD_LAYOUTS_H
#define KEYBOARD_LAYOUTS_H
#include <glib.h>

struct layout {
	char *lang;
	char *description;
};

void keyboard_layouts_init(GList **layouts, const char *filename);
void keyboard_layouts_finish(GList *layouts);

#endif /* KEYBOARD_LAYOUTS_H */
