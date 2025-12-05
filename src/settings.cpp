#include <QDebug>
#include <variant>
#include "log.h"
#include "settings.h"
#include "environment.h"
#include "xml.h"

//~ We try not to deal with raw pointers, but keeping *settings in this
// translation unit just helps not trickle it through lots of QWidget
// derived classes
static std::vector<std::shared_ptr<Setting>> *_settings;

void initSettings(std::vector<std::shared_ptr<Setting>> *settings)
{
    _settings = settings;

    // Appearance
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/name", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_STRING, ""));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/cornerRadius",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_INT, 8));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/dropShadows",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/icon", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_STRING, ""));

    // Behaviour
    settings->push_back(std::make_shared<Setting>("/labwc_config/placement/policy",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "Cascade"));

    // Mouse & Touchpad
    settings->push_back(std::make_shared<Setting>("XCURSOR_THEME", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_STRING, ""));

    settings->push_back(std::make_shared<Setting>("XCURSOR_SIZE", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_INT, 24));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/naturalScroll",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/leftHanded",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/pointerSpeed",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_FLOAT, 0.0f));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/accelProfile",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "Flat"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tap",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tapButtonMap",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "lrm"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tapAndDrag",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/dragLock",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/threeFingerDrag",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/middleEmulation",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/disableWhileTyping",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/clickMethod",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "none"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/scrollMethod",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "twoFinger"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/sendEventsMode",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "yes"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/scrollFactor",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_FLOAT, 0.0f));

    // Language
    settings->push_back(std::make_shared<Setting>("XKB_DEFAULT_LAYOUT", LAB_FILE_TYPE_ENVIRONMENT,

                                                  LAB_VALUE_TYPE_STRING, "us"));
}

Setting::Setting(QString name, enum settingFileType fileType, enum settingValueType valueType,
                 std::variant<int, float, QString> defaultValue)
    : m_name(name), m_fileType(fileType), m_valueType(valueType), m_value(defaultValue)
{
    m_valueOrigin = LAB_VALUE_ORIGIN_DEFAULT;

    // Use values from rc.xml if different from default
    if (m_fileType == LAB_FILE_TYPE_RCXML) {
        switch (m_valueType) {
            case LAB_VALUE_TYPE_INT: {
                int value = xml_get_int(m_name.toStdString().c_str());
                if (value != std::get<int>(m_value)) {
                    m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                    m_value = value;
                    info("[user-override] {}: {}", m_name.toStdString(), value);
                }
                break;
            }
            case LAB_VALUE_TYPE_FLOAT: {
                float value = xml_get_float(m_name.toStdString().c_str());
                if (value != std::get<float>(m_value)) {
                    m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                    m_value = value;
                    info("[user-override] {}: {}", m_name.toStdString(), value);
                }
                break;
            }
            case LAB_VALUE_TYPE_BOOL: {
                int value = xml_get_bool_text(m_name.toStdString().c_str());
                if (value != std::get<int>(m_value)) {
                    m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                    m_value = value;
                    info("[user-override] {}: {}", m_name.toStdString(), value);
                }
                break;
            }
            case LAB_VALUE_TYPE_STRING: {
                const char *value = xml_get(m_name.toStdString().c_str());
                if (value && QString(value) != std::get<QString>(m_value)) {
                    m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                    m_value = QString(value);
                    info("[user-override] {}: {}", m_name.toStdString(), value);
                }
                break;
            }
            default:
                break;
        }
    }

    // Use values from environment file if different from default
    if (m_fileType == LAB_FILE_TYPE_ENVIRONMENT) {
        switch (m_valueType) {
        case LAB_VALUE_TYPE_STRING: {
            QString value = QString(environmentGet(m_name));
            if (!value.isNull() && (value != std::get<QString>(m_value))) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value.toStdString());
            }
            break;
        }
        case LAB_VALUE_TYPE_INT: {
            bool success = false;
            int value = environmentGetInt(m_name);
            if (value == -1) {
                // There was no environment file - or it did not contain the key
                break;
            }
            if (value != std::get<int>(m_value)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value);
            }
            break;
        }
        case LAB_VALUE_TYPE_BOOL: {
            // do we need this?
            break;
        }
        default:
            break;
        }
    }
}

void Setting::setValue(std::variant<int, float, QString> value)
{
    if (value != m_value) {
        m_valueOrigin = LAB_VALUE_ORIGIN_CHANGED_IN_THIS_SESSION;
        m_value = value;
    }
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
        qDebug() << "warning: no settings with name" << name;
        return nullptr;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_STRING) {
        qDebug() << "getStr(): not valid string setting" << name;
    }
    return std::get<QString>(setting->value());
}

int getInt(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return -65535;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_INT) {
        qDebug() << "getInt(): not valid int setting" << name;
    }
    return std::get<int>(setting->value());
}

float getFloat(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return -65535;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_FLOAT) {
        qDebug() << "getFloat(): not valid float setting" << name;
    }
    return std::get<float>(setting->value());
}

/* Return -1 for error, which works will with setCurrentIndex() */
int getBool(QString name)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return -1;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_BOOL) {
        qDebug() << "getBool(): not valid bool setting" << name;
    }
    return std::get<int>(setting->value());
}

void setInt(QString name, int value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_INT) {
        qDebug() << "setInt(): not valid int setting" << name << value;
    }
    if (value != std::get<int>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value);
        xpath_add_node(name.toStdString().c_str());
        xml_set_num(name.toStdString().c_str(), value);
        setting->setValue(value);
    }
}

void setFloat(QString name, float value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_FLOAT) {
        qDebug() << "setFloat(): not valid float setting" << name << value;
    }
    if (value != std::get<float>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value);
        xpath_add_node(name.toStdString().c_str());
        xml_set_num(name.toStdString().c_str(), value);
        setting->setValue(value);
    }
}

void setStr(QString name, QString value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_STRING) {
        qDebug() << "setStr(): not valid string setting" << name << value;
    }
    if (value != std::get<QString>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value.toStdString());
        xpath_add_node(name.toStdString().c_str());
        xml_set(name.toStdString().c_str(), value.toStdString().c_str());
        setting->setValue(value);
    }
}

/**
 * parse_bool() - Parse boolean value of string.
 * @string: String to interpret. This check is case-insensitive.
 * @default_value: Default value to use if string is not a recognised boolean.
 *                 Use -1 to avoid setting a default value.
 *
 * Return: 0 for false; 1 for true; -1 for non-boolean
 */
int parseBool(const char *str, int defaultValue)
{
    if (!str)
        goto error_not_a_boolean;
    else if (!strcasecmp(str, "yes"))
        return 1;
    else if (!strcasecmp(str, "true"))
        return 1;
    else if (!strcasecmp(str, "on"))
        return 1;
    else if (!strcmp(str, "1"))
        return 1;
    else if (!strcasecmp(str, "no"))
        return 0;
    else if (!strcasecmp(str, "false"))
        return 0;
    else if (!strcasecmp(str, "off"))
        return 0;
    else if (!strcmp(str, "0"))
        return 0;
error_not_a_boolean:
    qDebug() << str << "is not a boolean value";
    return defaultValue;
}

void setBool(QString name, int value)
{
    std::shared_ptr<Setting> setting = retrieve(name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_BOOL) {
        qDebug() << "setBool(): not valid bool setting" << name << value;
    }
    if (value != std::get<int>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value);
        xpath_add_node(name.toStdString().c_str());
        xml_set(name.toStdString().c_str(), value ? "yes" : "no");
        setting->setValue(value);
    }
}

void setBoolfromString(QString name, QString value)
{
    int boolValue = parseBool(value.toStdString().c_str(), -1);
    setBool(name, boolValue);
}
