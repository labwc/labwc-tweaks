#pragma once
#include <memory>
#include <vector>
#include <QString>
#include "settings.h"

enum settingFileType {
    LAB_FILE_TYPE_UNKNOWN = 0,
    LAB_FILE_TYPE_RCXML,
    LAB_FILE_TYPE_ENVIRONMENT,
};

enum settingValueOrigin {
    LAB_VALUE_ORIGIN_UNKNOWN = 0,
    LAB_VALUE_ORIGIN_DEFAULT,
    LAB_VALUE_ORIGIN_USER_OVERRIDE,
    LAB_VALUE_ORIGIN_CHANGED_IN_THIS_SESSION,
};

enum settingValueType {
    LAB_VALUE_TYPE_UNKNOWN = 0,
    LAB_VALUE_TYPE_INT,
    LAB_VALUE_TYPE_BOOL,
    LAB_VALUE_TYPE_STRING,
};

static inline QString settingFileTypeName(enum settingFileType type)
{
    if (type == LAB_FILE_TYPE_RCXML)
        return "rc.xml";
    else if (type == LAB_FILE_TYPE_ENVIRONMENT)
        return "environment";
    return "unknown";
}

class Setting
{
public:
    Setting(QString name, enum settingFileType fileType, enum settingValueType valueType,
            std::variant<int, QString> defaultValue);
    void setValue(std::variant<int, QString> value);

private:
    QString m_name;
    enum settingFileType m_fileType;
    enum settingValueOrigin m_valueOrigin;
    enum settingValueType m_valueType;
    std::variant<int, QString> m_value;

public:
    // Getters
    QString name() const { return m_name; }
    enum settingFileType fileType() const { return m_fileType; }
    enum settingValueOrigin valueOrigin() const { return m_valueOrigin; }
    enum settingValueType valueType() const { return m_valueType; }
    std::variant<int, QString> value() const { return m_value; }
};

void initSettings(std::vector<std::shared_ptr<Setting>> *settings);
QString getStr(QString name);
int getInt(QString name);
int getBool(QString name);
void setInt(QString name, int value);
void setStr(QString name, QString value);
int parseBool(const char *str, int defaultValue);
void setBool(QString name, int value);
