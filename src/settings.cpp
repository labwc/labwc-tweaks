#include "settings.h"
#include <QDebug>
#include <variant>
#include "log.h"
#include "environment.h"
#include "macros.h"
#include "xml.h"

//~ We try not to deal with raw pointers, but keeping *settings in this
// translation unit just helps not trickle it through lots of QWidget
// derived classes
static std::vector<std::shared_ptr<Setting>> *_settings;

void settingsInit(std::vector<std::shared_ptr<Setting>> *settings)
{
    _settings = settings;

    // Appearance
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/name", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_STRING, ""));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/cornerRadius",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_INT, 8));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/dropShadows",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/dropShadowsOnTiled",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/icon", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_STRING, ""));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/maximizedDecoration",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "titlebar"));
    settings->push_back(std::make_shared<Setting>("/labwc_config/core/decoration",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "Server"));

    // Behaviour
    settings->push_back(std::make_shared<Setting>("/labwc_config/placement/policy",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "cascade"));
    settings->push_back(std::make_shared<Setting>("/labwc_config/focus/followMouse",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/focus/followMouseRequiresMovement",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/focus/raiseOnFocus",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/core/gap", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/snapping/cornerRange", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 50));
    settings->push_back(std::make_shared<Setting>("/labwc_config/snapping/topMaximize",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));
    settings->push_back(std::make_shared<Setting>("/labwc_config/snapping/notifyClient",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "always"));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resistance/screenEdgeStrength", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 20));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resistance/windowEdgeStrength", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 50));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resistance/unSnapTreshold", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 20));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resistance/unMaximizeTreshold", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 150));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resize/drawContents",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));
    settings->push_back(std::make_shared<Setting>("/labwc_config/theme/keepBorder",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resize/cornerRange", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 8)); // Default is half the titlebar height
    settings->push_back(std::make_shared<Setting>("/labwc_config/resize/resizeMinimumArea", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 8));
    settings->push_back(std::make_shared<Setting>("/labwc_config/resize/popupShow",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "Never"));
    settings->push_back(std::make_shared<Setting>("/labwc_config/magnifier/width", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 400));
    settings->push_back(std::make_shared<Setting>("/labwc_config/magnifier/height", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 400));
    settings->push_back(std::make_shared<Setting>("/labwc_config/magnifier/initScale", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_FLOAT, 2.0f));
    settings->push_back(std::make_shared<Setting>("/labwc_config/magnifier/increment", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_FLOAT, 0.2f));
    settings->push_back(std::make_shared<Setting>("/labwc_config/magnifier/useFilter",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));

    // Mouse & Touchpad
    settings->push_back(std::make_shared<Setting>("XCURSOR_THEME", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_STRING, ""));

    settings->push_back(std::make_shared<Setting>("XCURSOR_SIZE", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_INT, 24));

    // We're picking the "usual" default described the libinput documents, although recognise that
    // some devices do not always use these. We think that this approach makes for fewer
    // user-surprises.
    //
    // The exception is tap-to-click, which labwc enables be default because the original author
    // rather liked that behaviour.
    //
    // https://wayland.freedesktop.org/libinput/doc/latest/configuration.html

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/naturalScroll",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/leftHanded",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/pointerSpeed",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_FLOAT, 0.0f));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/accelProfile",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "adaptive"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tap",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tapButtonMap",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_STRING,
                                                  "lrm"));

    // Most devices have tap-and-drag enabled by default
    // https://wayland.freedesktop.org/libinput/doc/latest/tapping.html#tapndrag
    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/tapAndDrag",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/dragLock",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    // https://wayland.freedesktop.org/libinput/doc/latest/drag-3fg.html#drag-3fg
    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/threeFingerDrag",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(std::make_shared<Setting>("/labwc_config/libinput/device/middleEmulation",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));

    settings->push_back(
            std::make_shared<Setting>("/labwc_config/libinput/device/disableWhileTyping",
                                      LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 1));

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
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_FLOAT, 1.0f));

    // Keyboard
    settings->push_back(std::make_shared<Setting>("XKB_DEFAULT_LAYOUT", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_STRING, "us"));

    settings->push_back(std::make_shared<Setting>("/labwc_config/keyboard/numlock",
                                                  LAB_FILE_TYPE_RCXML, LAB_VALUE_TYPE_BOOL, 0));
    settings->push_back(std::make_shared<Setting>("/labwc_config/keyboard/repeatRate", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 25));
    settings->push_back(std::make_shared<Setting>("/labwc_config/keyboard/repeatDelay", LAB_FILE_TYPE_RCXML,
                                                  LAB_VALUE_TYPE_INT, 600));

    settings->push_back(std::make_shared<Setting>("XKB_DEFAULT_OPTIONS", LAB_FILE_TYPE_ENVIRONMENT,
                                                  LAB_VALUE_TYPE_STRING, ""));
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
    case LAB_FILE_TYPE_RCXML:
        xpath_add_node(name.toStdString().c_str());
        xml_set(name.toStdString().c_str(), value.toStdString().c_str());
        break;
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
    case LAB_FILE_TYPE_RCXML:
        xpath_add_node(name.toStdString().c_str());
        xml_set_num(name.toStdString().c_str(), value);
        break;
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
    case LAB_FILE_TYPE_RCXML:
        xpath_add_node(name.toStdString().c_str());
        xml_set(name.toStdString().c_str(), value ? "yes" : "no");
        break;
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
    case LAB_FILE_TYPE_RCXML:
        xpath_add_node(name.toStdString().c_str());
        xml_set_num(name.toStdString().c_str(), value);
        break;
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
