#pragma once
#include "settings.h"
#include <memory>
#include <vector>
#include <QString>
#include "setting.h"

void settingsInit(std::vector<std::shared_ptr<Setting>> *settings);

// Add new entries
void settingsAddXmlStr(QString name, QString defaultValue);
void settingsAddXmlInt(QString name, int defaultValue);
void settingsAddXmlBoo(QString name, bool defaultValue);
void settingsAddXmlFlt(QString name, float defaultValue);
void settingsAddEnvStr(QString name, QString defaultValue);
void settingsAddEnvInt(QString name, int defaultValue);

// Get values
QString getStr(QString name);
int getInt(QString name);
int getBool(QString name);
float getFloat(QString name);

// Set values for entries that already exist
void setStr(QString name, QString value);
void setInt(QString name, int value);
void setBool(QString name, int value);
void setFloat(QString name, float value);
