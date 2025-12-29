#pragma once
#include "settings.h"
#include <memory>
#include <vector>
#include <QString>
#include "setting.h"

void settingsInit(std::vector<std::shared_ptr<Setting>> *settings);

// Helpers to add entries in settings vector
void settingsAddXmlStr(QString name, QString defaultValue);
void settingsAddXmlInt(QString name, int defaultValue);

// Getters
QString getStr(QString name);
int getInt(QString name);
int getBool(QString name);
float getFloat(QString name);

// Setters (for entries that already exist)
void setStr(QString name, QString value);
void setInt(QString name, int value);
void setBool(QString name, int value);
void setFloat(QString name, float value);
