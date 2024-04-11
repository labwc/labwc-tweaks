// SPDX-License-Identifier: GPL-2.0-only
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "theme.h"

static void
rtrim(char **s)
{
	size_t len = strlen(*s);
	if (!len) {
		return;
	}
	char *end = *s + len - 1;
	while (end >= *s && isspace(*end)) {
		end--;
	}
	*(end + 1) = '\0';
}

static char *
string_strip(char *s)
{
	rtrim(&s);
	while (isspace(*s)) {
		s++;
	}
	return s;
}

static char *
get_value(char *line, const char *key)
{
	if (!line || !*line || line[0] == '#') {
		return NULL;
	}
	char *p = strchr(line, '=');
	if (!p) {
		return NULL;
	}
	*p = '\0';
	if (!!strcmp(key, string_strip(line))) {
		return NULL;
	}
	char *value = string_strip(++p);
	return value ? value : NULL;
}

void
environment_get(char *buffer, size_t size, const char *key)
{
	char filename[4096];
	snprintf(filename, sizeof(filename), "%s/%s", getenv("HOME"), ".config/labwc/environment");

	char *value = NULL;
	char *line = NULL;
	size_t len = 0;
	FILE *stream = fopen(filename, "r");
	if (!stream) {
		return;
	}

	while (getline(&line, &len, stream) != -1) {
		char *p = strrchr(line, '\n');
		if (p) {
			*p = '\0';
		}
		value = get_value(line, key);
		if (value) {
			snprintf(buffer, size, "%s", value);
			break;
		}
	}
	free(line);
	fclose(stream);
}

void
environment_set(const char *key, const char *value)
{
	/* set cursor for labwc  - should cover 'replace' or 'append' */
	char xcur[4096] = {0};
	strcpy(xcur, key);
	strcat(xcur, "=");
	char filename[4096];
	char bufname[4096];
	char *home = getenv("HOME");
	snprintf(filename, sizeof(filename), "%s/%s", home, ".config/labwc/environment");
	snprintf(bufname, sizeof(bufname), "%s/%s", home, ".config/labwc/buf");
	FILE *fe = fopen(filename, "r");
	FILE *fw = fopen(bufname, "a");
	if ((fe == NULL) || (fw == NULL)) {
		perror("Unable to open file!");
		return;
	}
	char chunk[128];
	while (fgets(chunk, sizeof(chunk), fe) != NULL) {
		if (strstr(chunk, xcur) != NULL) {
			continue;
		} else {
			fprintf(fw, "%s", chunk);
		}
	}
	fclose(fe);
	if (value) {
		fprintf(fw, "%s\n", strcat(xcur, value));
	}
	fclose(fw);
	rename(bufname, filename);
}

void
environment_set_num(const char *key, int value)
{
	char buffer[255];
	snprintf(buffer, 255, "%d", value);

	environment_set(key, buffer);
}

