/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef THEME_H
#define THEME_H
#include <stdbool.h>

struct theme {
	char *name;
	char *path;
};

struct themes {
	struct theme *data;
	int nr, alloc;
};

void theme_find(struct themes *themes, const char *middle, const char *end);
void theme_free_vector(struct themes *themes);

#endif /* THEME_H */
