#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <string.h>
#include "keyboard-layouts.h"

static int
cmp_layouts(const void *a, const void *b)
{
	return strcmp(((struct layout *)a)->lang, ((struct layout *)b)->lang);
}

void
keyboard_layouts_init(GList **layouts, const char *filename)
{
	bool in_layout_section = false;
	char *line = NULL;
	size_t len = 0;
	FILE *fp = fopen(filename, "r");

	if (!fp) {
		perror("Error opening file");
		exit (EXIT_FAILURE);
	}
	while (getline(&line, &len, fp) != -1) {
		char *p = strrchr(line, '\n');
		if (p) {
			*p = '\0';
		}
		if (line[0] == '\0') {
			continue;
		} else if (line[0] == '!') {
			in_layout_section = g_ascii_strncasecmp(line, "! layout", len) == 0;
		} else if (in_layout_section) {
			char **argv = g_strsplit(g_strstrip(line), " ", 2);
			struct layout *layout = calloc(1, sizeof(*layout));
			layout->lang = strdup(argv[0]);
			layout->description = strdup(g_strstrip(argv[1]));
			*layouts = g_list_append(*layouts, layout);
			g_strfreev(argv);
		}
	}
	free(line);
	fclose(fp);
	*layouts = g_list_sort(*layouts, (GCompareFunc)cmp_layouts);
}

void
keyboard_layouts_finish(GList *layouts)
{
	GList *iter;
	for (iter = layouts; iter; iter = iter->next) {
		struct layout *layout = (struct layout *)iter->data;
		g_free(layout->lang);
		g_free(layout->description);
		g_free(layout);
	}
	g_list_free(layouts);
}

