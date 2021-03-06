#ifndef __XML_H
#define __XML_H

void xml_init(const char *filename);
void xml_save(void);
void xml_finish(void);
void xml_set(char *nodename, char *value);
void xml_set_num(char *nodename, double value);
char *xml_get(char *nodename);
int xml_get_int(char *nodename);
int xml_get_bool_text(char *nodename);

#endif /* __XML_H */
