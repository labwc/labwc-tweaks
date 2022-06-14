#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <assert.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "tweaks.h"

static struct theme *
grow_vector_by_one_theme(struct themes *themes)
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

/**
 * process_dir - find themes and store them in vector
 * @themes: vector
 * @path: path to directory to search in
 * @filename: filename for which a successful stat will count as 'found theme',
 *	      using the schema @path/<themename>/@filename
 *
 * For example, to find the Numix openbox theme at
 * /usr/share/themes/Numix/openbox-3/themerc, the following parameters would
 * be used:
 * @path = "/usr/share/themes"
 * @filename = "openbox-3/themerc"
 */
static void
process_dir(struct themes *themes, const char *path, const char *filename)
{
	struct dirent *entry;
	DIR *dp;
	struct stat st;
	struct theme *theme = NULL;

	dp = opendir(path);
	if (!dp) {
		return;
	}
	while ((entry = readdir(dp))) {
		if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
			char buf[4096];
			snprintf(buf, sizeof(buf), "%s/%s/%s", path, entry->d_name, filename);
			/* filter 'hicolor' as it is not a complete icon set */
			if (strstr(buf, "hicolor") != NULL) {
				continue;
			}
			if (!stat(buf, &st)) {
				theme = grow_vector_by_one_theme(themes);
				theme->name = strdup(entry->d_name);
				theme->path = strdup(buf);
			}
		}
	}
	closedir(dp);
}

static bool
vector_contains(struct themes *themes, const char *needle)
{
	assert(needle);
	for (int i = 0; i < themes->nr; ++i) {
		struct theme *theme = themes->data + i;
		if (!theme || !theme->name) {
			continue;
		}
		if ((!strcmp(theme->name, needle))) {
			return true;
		}
	}
	return false;
}

/* Sort system themes in alphabetical order */
static int
compare(const void *a, const void *b)
{
	const struct theme *theme_a = (struct theme *)a;
	const struct theme *theme_b = (struct theme *)b;
	return strcasecmp(theme_a->name, theme_b->name);
}

static struct {
	const char *prefix;
	const char *path;
} dirs[] = {
	{ "XDG_DATA_HOME", "" },
	{ "HOME", ".local/share" },
	{ "XDG_DATA_DIRS", "" },
	{ NULL, "/usr/share" },
	{ NULL, "/usr/local/share" },
	{ NULL, "/opt/share" },
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void
find_themes(struct themes *themes, const char *middle, const char *end)
{
	char path[4096];
	for (uint32_t i = 0; i < ARRAY_SIZE(dirs); ++i) {
		if (dirs[i].prefix) {
			char *prefix = getenv(dirs[i].prefix);
			if (!prefix) {
				continue;
			}
			snprintf(path, sizeof(path), "%s/%s/%s", prefix, dirs[i].path, middle);
		} else {
			snprintf(path, sizeof(path), "%s/%s", dirs[i].path, middle);
		}

		/*
		 * Find themes by recursively reading directories at
		 * "$DATA_DIR/@middle" and then checking for
		 * "$DATA_DIR/@middle/<themename>/@end"
		 */
		process_dir(themes, path, end);
	}

	/*
	 * In some distros Adwaita is built-in to gtk+-3.0 and subsequently no
	 * theme dir exists. In this case we add it manually.
	 */
	if (!vector_contains(themes, "Adwaita")) {
		struct theme *theme = grow_vector_by_one_theme(themes);
		theme->name = strdup("Adwaita");
		theme->path = NULL;		
	}

	qsort(themes->data, themes->nr, sizeof(struct theme), compare);
}
