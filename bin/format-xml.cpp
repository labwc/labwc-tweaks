// SPDX-License-Identifier: GPL-2.0-only
/*
 * Helper to format XML the way libxml2 does it - because it helps spot
 * differences when working labwc-tweaks.
 *
 * Copyright (C) Johan Malm 2026
 */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/xml-prettify.cpp"

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}
	const char *filename = argv[1];

	/* Use XML_PARSE_NOBLANKS for xmlSaveFormatFile() to indent nicely */
	xmlDoc *doc = xmlReadFile(filename, NULL, XML_PARSE_NOBLANKS);
	if (!doc) {
		fprintf(stderr, "bad file '%s'\n", filename);
		return EXIT_FAILURE;
	}
	xmlSaveFormatFile(filename, doc, 1);
	xmlFreeDoc(doc);

	xml_prettify(filename);

	return EXIT_SUCCESS;
}
