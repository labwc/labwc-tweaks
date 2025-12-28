#pragma once
#include "settings.h"
#include <memory>
#include <vector>
#include <QString>
#include "setting.h"

void initSettings(std::vector<std::shared_ptr<Setting>> *settings);

QString getStr(QString name);
int getInt(QString name);
int getBool(QString name);
float getFloat(QString name);

void setStr(QString name, QString value);
void setInt(QString name, int value);
void setBool(QString name, int value);
void setFloat(QString name, float value);
