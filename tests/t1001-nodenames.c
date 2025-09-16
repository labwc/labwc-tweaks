#define _POSIX_C_SOURCE 200809L
#include <glib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "tap.h"
#include "../src/xml.c"

static char template[] =
	"<?xml version=\"1.0\"?>\n"
	"<labwc_config>\n"
	"  <core>\n"
	"    <gap/>\n"
	"  </core>\n"
	"</labwc_config>\n";

void test(const char *nodename, const char *expect)
{
	bool is_equal = strcmp(nodename, expect) == 0;
	ok1(is_equal);
	if (!is_equal)
		fprintf(stderr, "%s\n%s\n", nodename, expect);
}

int main(int argc, char **argv)
{
	char in[] = "/tmp/t1001-expect_XXXXXX";
	static char buffer[256] = { 0 };

	plan(1);

	int fd = mkstemp(in);
	if (fd < 0)
		exit(EXIT_FAILURE);
	(void)write(fd, template, sizeof(template) - 1);

	/* test 1 */
	diag("generate simple xpath style nodename");
	xml_init(in);
	xmlNode *node = xpath_get_node((xmlChar *)"/labwc_config/core/gap");
	char *name = nodename(node, buffer, sizeof(buffer));
	xml_finish();
	test(name, "/labwc_config/core/gap");

	unlink(in);
	return exit_status();
}

