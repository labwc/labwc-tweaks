#include <QString>
#include <QVariant>
#include "log.h"
#include "settings.h"
#include "environment.h"
#include "macros.h"
#include "nodename.h"
#include "xml.h"

bool isValidBool(int value)
{
    return value != -1;
}

Setting::Setting(QString name, enum settingFileType fileType, enum settingValueType valueType,
                 QVariant defaultValue)
    : m_name(name), m_fileType(fileType), m_valueType(valueType), m_value(defaultValue)
{
    m_valueOrigin = LAB_VALUE_ORIGIN_DEFAULT;

    std::string nodename, truncatedXPath;
    if (m_fileType == LAB_FILE_TYPE_RCXML) {
        nodename = nodenameFromXPath(name.toStdString());
        truncatedXPath = name.replace("/labwc_config", "").toStdString();
    }

    // Use values from rc.xml if different from default
    if (m_fileType == LAB_FILE_TYPE_RCXML) {
        switch (m_valueType) {
        case LAB_VALUE_TYPE_STRING: {
            const char *value = xml_get(nodename.c_str());
            if (value && QString::compare(value, m_value.toString(), Qt::CaseInsensitive)) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = QString(value);
                info("from rc.xml use {}={}", truncatedXPath, value);
            }
            break;
        }
        case LAB_VALUE_TYPE_INT: {
            int value = xml_get_int(nodename.c_str());
            if (value != LAB_INVALID && value != m_value.toInt()) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("from rc.xml use {}={}", truncatedXPath, value);
            }
            break;
        }
        case LAB_VALUE_TYPE_FLOAT: {
            float value = xml_get_float(nodename.c_str());
            if (value != LAB_INVALID && value != m_value.toFloat()) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("from rc.xml use {}={}", truncatedXPath, value);
            }
            break;
        }
        case LAB_VALUE_TYPE_BOOL: {
            int value = xml_get_bool_text(nodename.c_str());
            if (isValidBool(value) && value != m_value.toInt()) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("from rc.xml use {}={}", truncatedXPath, value ? "true" : "false");
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
            if (!value.isNull() && (value != m_value.toString())) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("from environment file use {}={}", m_name.toStdString(), value.toStdString());
            }
            break;
        }
        case LAB_VALUE_TYPE_INT: {
            int value = environmentGetInt(m_name);
            if (value == -1) {
                // There was no environment file - or it did not contain the key
                break;
            }
            if (value != m_value.toInt()) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("from environment file use {}={}", m_name.toStdString(), value);
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

void Setting::setValue(QVariant value)
{
    if (value != m_value) {
        m_valueOrigin = LAB_VALUE_ORIGIN_CHANGED_IN_THIS_SESSION;
        m_value = value;
    }
}
