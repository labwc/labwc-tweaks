#include <QString>
#include <variant>
#include "log.h"
#include "settings.h"
#include "environment.h"
#include "macros.h"
#include "xml.h"

bool isValidBool(int value)
{
    return value != -1;
}

Setting::Setting(QString name, enum settingFileType fileType, enum settingValueType valueType,
                 std::variant<int, float, QString> defaultValue)
    : m_name(name), m_fileType(fileType), m_valueType(valueType), m_value(defaultValue)
{
    m_valueOrigin = LAB_VALUE_ORIGIN_DEFAULT;

    // Use values from rc.xml if different from default
    if (m_fileType == LAB_FILE_TYPE_RCXML) {
        switch (m_valueType) {
        case LAB_VALUE_TYPE_STRING: {
            const char *value = xml_get(m_name.toStdString().c_str());
            if (value && QString(value) != std::get<QString>(m_value)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = QString(value);
                info("[user-override] {}: {}", m_name.toStdString(), value);
            }
            break;
        }
        case LAB_VALUE_TYPE_INT: {
            int value = xml_get_int(m_name.toStdString().c_str());
            if (value != LAB_INVALID && value != std::get<int>(m_value)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value);
            }
            break;
        }
        case LAB_VALUE_TYPE_FLOAT: {
            float value = xml_get_float(m_name.toStdString().c_str());
            if (value != LAB_INVALID && value != value != std::get<float>(m_value)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value);
            }
            break;
        }
        case LAB_VALUE_TYPE_BOOL: {
            int value = xml_get_bool_text(m_name.toStdString().c_str());
            if (isValidBool(value) && value != std::get<int>(m_value)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
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
