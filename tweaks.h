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

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void find_themes(struct themes *themes, const char *middle, const char *end);

void xml_init(const char *filename);
void xml_save(void);
void xml_finish(void);
void xml_set(char *nodename, char *value);
void xml_set_num(char *nodename, double value);
char *xml_get(char *nodename);
int xml_get_int(char *nodename);
int xml_get_bool_text(char *nodename);

#endif /* __TWEAKS_H */
