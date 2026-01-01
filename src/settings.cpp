#include "settings.h"
#include <QDebug>
#include <string>
#include <variant>
#include "log.h"
#include "environment.h"
#include "macros.h"
#include "xml.h"
#include "nodename.h"

//~ We try not to deal with raw pointers, but keeping *settings in this
// translation unit just helps not trickle it through lots of QWidget
// derived classes
static std::vector<std::shared_ptr<Setting>> *_settings;

static void add(QString name, enum settingFileType fileType, enum settingValueType valueType,
                std::variant<int, float, QString> defaultValue)
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

    // Behaviour
    settingsAddXmlStr("/labwc_config/placement/policy", "cascade");
    settingsAddXmlBoo("/labwc_config/focus/followMouse", false);
    settingsAddXmlBoo("/labwc_config/focus/followMouseRequiresMovement", false);
    settingsAddXmlBoo("/labwc_config/focus/raiseOnFocus", false);
    settingsAddXmlInt("/labwc_config/core/gap", 0);
    settingsAddXmlInt("/labwc_config/snapping/cornerRange", 50);
    settingsAddXmlBoo("/labwc_config/snapping/topMaximize", true);
    settingsAddXmlStr("/labwc_config/snapping/notifyClient", "always");
    settingsAddXmlInt("/labwc_config/resistance/screenEdgeStrength", 20);
    settingsAddXmlInt("/labwc_config/resistance/windowEdgeStrength", 50);
    settingsAddXmlInt("/labwc_config/resistance/unSnapTreshold", 20);
    settingsAddXmlInt("/labwc_config/resistance/unMaximizeTreshold", 150);
    settingsAddXmlBoo("/labwc_config/resize/drawContents", true);
    settingsAddXmlBoo("/labwc_config/theme/keepBorder", true);
    settingsAddXmlInt("/labwc_config/resize/cornerRange", 8);
    settingsAddXmlInt("/labwc_config/resize/resizeMinimumArea", 8);
    settingsAddXmlStr("/labwc_config/resize/popupShow", "never");
    settingsAddXmlInt("/labwc_config/magnifier/width", 400);
    settingsAddXmlInt("/labwc_config/magnifier/height", 400);
    settingsAddXmlFlt("/labwc_config/magnifier/initScale", 2.0f);
    settingsAddXmlFlt("/labwc_config/magnifier/increment", 0.2f);
    settingsAddXmlBoo("/labwc_config/magnifier/useFilter", true);

    // Mouse & Touchpad
    settingsAddEnvStr("XCURSOR_THEME", "");
    settingsAddEnvInt("XCURSOR_SIZE", 24);

    // We're picking the "usual" default described the libinput documents, although recognise that
    // some devices do not always use these. We think that this approach makes for fewer
    // user-surprises.
    //
    // The exception is tap-to-click, which labwc enables be default because the original author
    // rather liked that behaviour.
    //
    // https://wayland.freedesktop.org/libinput/doc/latest/configuration.html

    settingsAddXmlBoo("/labwc_config/libinput/device/naturalScroll", false);
    settingsAddXmlBoo("/labwc_config/libinput/device/leftHanded", false);
    settingsAddXmlFlt("/labwc_config/libinput/device/pointerSpeed", 0.0f);
    settingsAddXmlStr("/labwc_config/libinput/device/accelProfile", "adaptive");
    settingsAddXmlBoo("/labwc_config/libinput/device/tap", true);
    settingsAddXmlStr("/labwc_config/libinput/device/tapButtonMap", "lrm");

    // Most devices have tap-and-drag enabled by default
    // https://wayland.freedesktop.org/libinput/doc/latest/tapping.html#tapndrag
    settingsAddXmlBoo("/labwc_config/libinput/device/tapAndDrag", true);
    settingsAddXmlBoo("/labwc_config/libinput/device/dragLock", false);

    // https://wayland.freedesktop.org/libinput/doc/latest/drag-3fg.html#drag-3fg
    settingsAddXmlBoo("/labwc_config/libinput/device/threeFingerDrag", false);
    settingsAddXmlBoo("/labwc_config/libinput/device/middleEmulation", false);
    settingsAddXmlBoo("/labwc_config/libinput/device/disableWhileTyping", true);
    settingsAddXmlStr("/labwc_config/libinput/device/clickMethod", "none");
    settingsAddXmlStr("/labwc_config/libinput/device/scrollMethod", "twoFinger");
    settingsAddXmlStr("/labwc_config/libinput/device/sendEventsMode", "yes");
    settingsAddXmlFlt("/labwc_config/libinput/device/scrollFactor", 1.0f);

    // Keyboard
    settingsAddEnvStr("XKB_DEFAULT_LAYOUT", "us");
    settingsAddXmlBoo("/labwc_config/keyboard/numlock", false);
    settingsAddXmlInt("/labwc_config/keyboard/repeatRate", 25);
    settingsAddXmlInt("/labwc_config/keyboard/repeatDelay", 600);
    settingsAddEnvStr("XKB_DEFAULT_OPTIONS", "");
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
    return std::get<QString>(setting->value());
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
    return std::get<int>(setting->value());
}

/* Return -1 for warnor, which works will with setCurrentIndex() */
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
    return std::get<int>(setting->value());
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
    return std::get<float>(setting->value());
}

//
// The setters below are for key=value pairs in "rc.xml" and "environment". More complex
// configuration involving objects like `<keybind>` cannot be managed through these.
//

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
    if (value == std::get<QString>(setting->value())) {
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
    if (value == std::get<int>(setting->value())) {
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
    if (value == std::get<int>(setting->value())) {
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
    if (value == std::get<float>(setting->value())) {
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
