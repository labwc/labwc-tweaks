/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <QString>

QString environment_get(QString key);
int environment_get_int(QString key);

void environment_set(const char *key, const char *value);
void environment_set_num(const char *key, int value);

void environmentInit(std::string filename);
void environmentSave(std::string filename);

#endif /* ENVIRONMENT_H */
