#include "settings.h"
#include <QDebug>
#include <QVariant>
#include <string>
#include "log.h"
#include "environment.h"
#include "macros.h"
#include "xml.h"
#include "nodename.h"

/*~
 * We try not to deal with raw pointers, but keeping *settings in this translation unit just helps
 * not trickle it through to lots of QWidget derived classes.
 */
static std::vector<std::shared_ptr<Setting>> *_settings;

static void add(QString name, enum settingFileType fileType, enum settingValueType valueType,
                QVariant defaultValue)
{
    _settings->push_back(std::make_shared<Setting>(name, fileType, valueType, defaultValue));
}

// rc.xml config file helpers
void settingsAddXmlStr(QString name, QString defaultValue)
{
    add(name, LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING, defaultValue);
}

void settingsAddXmlInt(QString name, int defaultValue)
{
    add(name, LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_INT, defaultValue);
}

void settingsAddXmlBoo(QString name, bool defaultValue)
{
    add(name, LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, defaultValue);
}

void settingsAddXmlFlt(QString name, float defaultValue)
{
    add(name, LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_FLOAT, defaultValue);
}

// environment file helpers
void settingsAddEnvStr(QString name, QString defaultValue)
{
    add(name, LAB_FILE_TYPE_ENVIRONMENT, LAB_VALUE_TYPE_STRING, defaultValue);
}

void settingsAddEnvInt(QString name, int defaultValue)
{
    add(name, LAB_FILE_TYPE_ENVIRONMENT, LAB_VALUE_TYPE_INT, defaultValue);
}

void settingsInit(std::vector<std::shared_ptr<Setting>> *settings)
{
    _settings = settings;
}

static std::shared_ptr<Setting> retrieve(QString name)
{
    for (auto &setting : *_settings) {
        if (name == setting->name()) {
            return setting;
        }
    }
    return nullptr;
}

QString getStr(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return nullptr;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_STRING) {
        warn("not a valid string setting '{}'", name.toStdString());
        return nullptr;
    }
    QString ret = setting->value().toString();
    return ret;
}

int getInt(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return LAB_INVALID;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_INT) {
        warn("not a valid int setting '{}'", name.toStdString());
        return LAB_INVALID;
    }
    int ret = setting->value().toInt();
    return ret;
}

/* Return -1 for error because this works well with setCurrentIndex() */
int getBool(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return -1;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_BOOL) {
        warn("not a valid boolean setting '{}'", name.toStdString());
        return LAB_INVALID;
    }
    int ret = setting->value().toInt();
    return ret;
}

float getFloat(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return LAB_INVALID;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_FLOAT) {
        warn("not a valid float setting '{}'", name.toStdString());
        return LAB_INVALID;
    }
    float ret = setting->value().toFloat();
    return ret;
}

/*~
 * The setters below are for key=value pairs in "rc.xml" and "environment". More complex
 * configuration involving objects like `<keybind>` cannot be managed through these.
 */
void setStr(QString name, QString value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_STRING) {
        warn("not a valid string setting '{}'", name.toStdString());
        return;
    }
    if (value == setting->value().toString()) {
        return;
    }
    switch (setting->fileType()) {
    case LAB_FILE_TYPE_RCXML: {
        xpath_add_node(name.toStdString().c_str());
        std::string nodename = nodenameFromXPath(name.toStdString());
        xml_set(nodename.c_str(), value.toStdString().c_str());
        break;
    }
    case LAB_FILE_TYPE_ENVIRONMENT:
        environmentSet(name, value);
        break;
    case LAB_FILE_TYPE_UNKNOWN:
    default:
        warn("cannot handle file type associated with '{}'", name.toStdString());
    }
    setting->setValue(value);
    info("'{} has changed to '{}'", name.toStdString(), value.toStdString());
}

void setInt(QString name, int value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_INT) {
        warn("not a valid int setting '{}'", name.toStdString());
        return;
    }
    if (value == setting->value().toInt()) {
        return;
    }
    switch (setting->fileType()) {
    case LAB_FILE_TYPE_RCXML: {
        xpath_add_node(name.toStdString().c_str());
        std::string nodename = nodenameFromXPath(name.toStdString());
        xml_set_num(nodename.c_str(), value);
        break;
    }
    case LAB_FILE_TYPE_ENVIRONMENT:
        environmentSetInt(name, value);
        break;
    case LAB_FILE_TYPE_UNKNOWN:
    default:
        warn("cannot handle file type associated with '{}'", name.toStdString());
    }
    setting->setValue(value);
    info("'{} has changed to '{}'", name.toStdString(), value);
}

void setBool(QString name, int value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_BOOL) {
        warn("not a valid boolean setting '{}'", name.toStdString());
        return;
    }
    if (value == setting->value().toInt()) {
        return;
    }
    switch (setting->fileType()) {
    case LAB_FILE_TYPE_RCXML: {
        xpath_add_node(name.toStdString().c_str());
        std::string nodename = nodenameFromXPath(name.toStdString());
        xml_set(nodename.c_str(), value ? "yes" : "no");
        break;
    }
    case LAB_FILE_TYPE_ENVIRONMENT:
        environmentSetInt(name, value);
        break;
    case LAB_FILE_TYPE_UNKNOWN:
    default:
        warn("cannot handle file type associated with '{}'", name.toStdString());
    }
    setting->setValue(value);
    info("'{} has changed to '{}'", name.toStdString(), value);
}

void setFloat(QString name, float value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        warn("no setting with name '{}'", name.toStdString());
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_FLOAT) {
        warn("not a valid float setting '{}'", name.toStdString());
        return;
    }
    if (value == setting->value().toFloat()) {
        return;
    }
    switch (setting->fileType()) {
    case LAB_FILE_TYPE_RCXML: {
        xpath_add_node(name.toStdString().c_str());
        std::string nodename = nodenameFromXPath(name.toStdString());
        xml_set_num(nodename.c_str(), value);
        break;
    }
    case LAB_FILE_TYPE_ENVIRONMENT:
        warn("do not yet support setting floats in environment file");
        break;
    case LAB_FILE_TYPE_UNKNOWN:
    default:
        warn("cannot handle file type associated with '{}'", name.toStdString());
    }
    setting->setValue(value);
    info("'{} has changed to '{}'", name.toStdString(), value);
}
