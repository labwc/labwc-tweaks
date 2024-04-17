#define _POSIX_C_SOURCE 200809L
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "tap.h"
#include "../src/xml.h"

static char template[] =
	"<?xml version=\"1.0\"?>\n"
	"<labwc_config>\n"
	"  <core>\n"
	"    <gap/>\n"
	"  </core>\n"
	"</labwc_config>\n";

void show_diff(const char *filename, const char *buf, size_t size)
{
	char cmd[1000];
	snprintf(cmd, sizeof(cmd), "diff -u - %s >&2", filename);
	FILE *f = popen(cmd, "w");
	fwrite(buf, size, 1, f);
	pclose(f);
}

void test(const char *filename, const char *expect)
{
	gsize length; gchar *actual;
	g_file_get_contents(filename, &actual, &length, NULL);
	bool is_equal = strcmp(actual, expect) == 0;
	ok1(is_equal);
	if (!is_equal)
		show_diff(filename, expect, strlen(expect));
	g_free(actual);
}

int main(int argc, char **argv)
{
	char in[] = "/tmp/t1000-expect_XXXXXX";
	char out[] = "/tmp/t1000-actual";

	plan(4);

	int fd = mkstemp(in);
	if (fd < 0)
		exit(EXIT_FAILURE);
	write(fd, template, sizeof(template) - 1);

	/* test 1 */
	diag("add node <theme><cornerradius> using xpath (lowercase)");
	xml_init(in);
	xpath_add_node("/labwc_config/theme/cornerradius");
	xml_save_as(out);
	xml_finish();
	test(out,
		"<?xml version=\"1.0\"?>\n"
		"<labwc_config>\n"
		"  <core>\n"
		"    <gap/>\n"
		"  </core>\n"
		"  <theme>\n"
		"    <cornerradius/>\n"
		"  </theme>\n"
		"</labwc_config>\n");

	/* test 2 */
	diag("add node <theme><cornerRadius> using xpath (camelCase)");
	xml_init(in);
	xpath_add_node("/labwc_config/theme/cornerRadius");
	xml_save_as(out);
	xml_finish();
	test(out,
		"<?xml version=\"1.0\"?>\n"
		"<labwc_config>\n"
		"  <core>\n"
		"    <gap/>\n"
		"  </core>\n"
		"  <theme>\n"
		"    <cornerRadius/>\n"
		"  </theme>\n"
		"</labwc_config>\n");

	/* test 3 */
	diag("check xpath does not add duplicate entries - when identical");
	xml_init(in);
	xpath_add_node("/labwc_config/theme/cornerradius");
	xpath_add_node("/labwc_config/theme/cornerradius");
	xml_save_as(out);
	xml_finish();
	test(out,
		"<?xml version=\"1.0\"?>\n"
		"<labwc_config>\n"
		"  <core>\n"
		"    <gap/>\n"
		"  </core>\n"
		"  <theme>\n"
		"    <cornerradius/>\n"
		"  </theme>\n"
		"</labwc_config>\n");

	/* test 4 */
	diag("check xpath does not add duplicate entries - even if they have differing capitalisation");
	xml_init(in);
	xpath_add_node("/labwc_config/theme/cornerradius");
	xpath_add_node("/labwc_config/theme/Cornerradius");
	xml_save_as(out);
	xml_finish();
	test(out,
		"<?xml version=\"1.0\"?>\n"
		"<labwc_config>\n"
		"  <core>\n"
		"    <gap/>\n"
		"  </core>\n"
		"  <theme>\n"
		"    <cornerradius/>\n"
		"  </theme>\n"
		"</labwc_config>\n");

	unlink(in);
	unlink(out);
	return exit_status();
}

