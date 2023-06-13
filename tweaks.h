/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __TWEAKS_H
#define __TWEAKS_H
#include <stdbool.h>

struct theme {
	char *name;
	char *path;
};

struct themes {
	struct theme *data;
	int nr, alloc;
};

void find_themes(struct themes *themes, const char *middle, const char *end);

#endif /* __TWEAKS_H */
