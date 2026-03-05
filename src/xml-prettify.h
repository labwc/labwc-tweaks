/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef XML_PRETTIFY_H
#define XML_PRETTIFY_H

/*
 * Add some labwc specific formatting to make the xml prettier and align with
 * the default files in labwc/docs/
 *
 * Specifically:
 *   - Remove the <?xml version="1.0"?> line
 *   - Convert <foo/> to <foo />
 */
void xml_prettify(const char *filename);

#endif /* XML_PRETTIFY_H */
