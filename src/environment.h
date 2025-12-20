/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <QString>

QString environmentGet(QString key);
int environmentGetInt(QString key);
void environmentSet(QString key, QString value);
void environmentSetInt(QString key, int value);
void environmentInit(std::string filename);
void environmentSave(void);

#endif /* ENVIRONMENT_H */
