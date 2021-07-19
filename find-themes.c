#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "tweaks.h"

static struct
theme *grow_vector_by_one_theme(struct themes *themes)
{
	if (themes->nr == themes->alloc) {
		themes->alloc = (themes->alloc + 16) * 2;
		themes->data = realloc(themes->data, themes->alloc * sizeof(struct theme));
	}
	struct theme *theme = themes->data + themes->nr;
	memset(theme, 0, sizeof(*theme));
	themes->nr++;
	return theme;
}

void
find_themes(struct themes *themes, const char *path)
{
	struct dirent *entry;
	DIR *dp;
	struct stat st;
	struct theme *theme;

	dp = opendir(path);
	if (!dp) {
		return;
	}
	while ((entry = readdir(dp))) {
		if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
			char buf[PATH_MAX];
			snprintf(buf, sizeof(buf), "%s/%s/openbox-3/themerc",
				 path, entry->d_name);
			if (!stat(buf, &st)) {
				theme = grow_vector_by_one_theme(themes);
				theme->name = strdup(entry->d_name);
				theme->path = strdup(buf);
			}
		}
	}
	closedir(dp);
}
