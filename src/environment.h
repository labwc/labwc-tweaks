/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <QString>

QString environmentGet(QString key);
int environmentGetInt(QString key);
void environmentSet(const char *key, const char *value);
void environmentSetInt(const char *key, int value);
void environmentInit(std::string filename);
void environmentSave(std::string filename);

#endif /* ENVIRONMENT_H */
