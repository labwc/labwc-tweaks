/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <QString>

QString environment_get(const char *key);
void environment_set(const char *key, const char *value);
void environment_set_num(const char *key, int value);

#endif /* ENVIRONMENT_H */
