#ifndef KEYBOARD_LAYOUTS_H
#define KEYBOARD_LAYOUTS_H
#include <gtk/gtk.h>

struct layout {
	char *lang;
	char *description;
};

void keyboard_layouts_init(GList **layouts, const char *filename);
void keyboard_layouts_finish(GList *layouts);

#endif /* KEYBOARD_LAYOUTS_H */
