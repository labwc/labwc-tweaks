/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __XML_H
#define __XML_H

void xml_init(const char *filename);
void xml_save(void);
void xml_save_as(const char *filename);
void xml_finish(void);
void xml_set(char *nodename, char *value);
void xml_set_num(char *nodename, double value);
char *xml_get(char *nodename);
int xml_get_int(char *nodename);
int xml_get_bool_text(char *nodename);

/**
 * xpath_get_content() - Get content of node specified by xpath
 * @xpath_expr: xpath expression for node
 */
char *xpath_get_content(char *xpath_expr);

/**
 * xpath_add_node - add xml nodes from xpath
 * @xpath_expr: xpath expression for new node
 * For example xpath_expr="/labwc_config/a/b/c" creates
 * <labwc_config><a><b><c /></b></a></labwc_config>
 */
void xpath_add_node(char *xpath_expr);

#endif /* __XML_H */
