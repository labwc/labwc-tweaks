#include <QDebug>
#include <variant>
#include "log.h"
#include "settings.h"

extern "C" {
#include "xml.h"
}

void initSettings(std::vector<std::shared_ptr<Setting>> &settings)
{
    // Appearance
    settings.push_back(std::make_shared<Setting>("/labwc_config/theme/name", LAB_FILE_TYPE_RCXML,
                                                 LAB_VALUE_TYPE_STRING, ""));
    settings.push_back(std::make_shared<Setting>("/labwc_config/theme/cornerRadius",
                                                 LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_INT, 8));
    settings.push_back(std::make_shared<Setting>("/labwc_config/theme/dropShadows",
                                                 LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings.push_back(std::make_shared<Setting>("/labwc_config/theme/icon", LAB_FILE_TYPE_RCXML,
                                                 LAB_VALUE_TYPE_STRING, ""));

    // Behaviour
    settings.push_back(std::make_shared<Setting>("/labwc_config/placement/policy",
                                                 LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                 "Cascade"));

    // Mouse & Touchpad
    settings.push_back(std::make_shared<Setting>("XCURSOR_THEME", LAB_FILE_TYPE_ENVIRONMENT,
                                                 LAB_VALUE_TYPE_STRING, "Adwaita"));
    settings.push_back(std::make_shared<Setting>("XCURSOR_SIZE", LAB_FILE_TYPE_ENVIRONMENT,
                                                 LAB_VALUE_TYPE_INT, 24));
    settings.push_back(std::make_shared<Setting>("/labwc_config/libinput/device/naturalScroll",
                                                 LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    // Language
    settings.push_back(std::make_shared<Setting>("XKB_DEFAULT_LAYOUT", LAB_FILE_TYPE_ENVIRONMENT,
                                                 LAB_VALUE_TYPE_STRING, "us"));
}

Setting::Setting(QString name, enum settingFileType fileType, enum settingValueType valueType,
                 std::variant<int, QString> defaultValue)
    : m_name(name), m_fileType(fileType), m_valueType(valueType), m_value(defaultValue)
{
    m_valueOrigin = LAB_VALUE_ORIGIN_DEFAULT;

    if (m_fileType == LAB_FILE_TYPE_RCXML) {
        switch (m_valueType) {
        case LAB_VALUE_TYPE_STRING: {
            QString value = QString(xml_get(m_name.toStdString().c_str()));
            if (!value.isNull() && (value != std::get<QString>(m_value))) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value.toStdString());
            }
            break;
        }
        case LAB_VALUE_TYPE_INT: {
            int value = xml_get_int(m_name.toStdString().c_str());
            if (value && (value != std::get<int>(m_value))) {
                m_valueOrigin = LAB_VALUE_ORIGIN_USER_OVERRIDE;
                m_value = value;
                info("[user-override] {}: {}", m_name.toStdString(), value);
            }
            break;
        }
        case LAB_VALUE_TYPE_BOOL: {
            int value = xml_get_bool_text(m_name.toStdString().c_str());
            if (value != -1 && value != std::get<int>(m_value)) {
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
};

std::shared_ptr<Setting> retrieve(std::vector<std::shared_ptr<Setting>> &settings, QString name)
{
    for (auto &setting : settings) {
        if (name == setting->name()) {
            return setting;
        }
    }
    return nullptr;
}
