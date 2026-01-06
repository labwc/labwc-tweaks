#include "mouse.h"
#include "environment.h"
#include "find-themes.h"
#include "macros.h"
#include "pair.h"
#include "settings.h"
#include "./ui_mouse.h"

Mouse::Mouse(QWidget *parent) : QWidget(parent), ui(new Ui::pageMouse)
{
    ui->setupUi(this);
}

Mouse::~Mouse()
{
    delete ui;
}

void Mouse::activate()
{
    /* Cursor Theme */
    settingsAddEnvStr("XCURSOR_THEME", "");
    QStringList cursorThemes = findIconThemes(LAB_ICON_THEME_TYPE_CURSOR);
    ui->cursorTheme->addItems(cursorThemes);
    ui->cursorTheme->setCurrentIndex(cursorThemes.indexOf(getStr("XCURSOR_THEME")));

    /* Cursor Size */
    settingsAddEnvInt("XCURSOR_SIZE", 24);
    ui->cursorSize->setValue(getInt("XCURSOR_SIZE"));

    /*~
     * For libinput settings we pick the default described the libinput documents, although
     * recognise that this is not 100% consistent across all devices. We think that this approach
     * makes for the least amount of user-surprises and it seems consistent with how some big
     * established compositors handle this.
     *
     * The exception is tap-to-click, which labwc enables by default for historic reasons.
     *
     * Ref:
     *   - https://wayland.freedesktop.org/libinput/doc/latest/configuration.html
     */

    /* Natural Scroll */
    settingsAddXmlBoo("/labwc_config/libinput/device/naturalScroll", false);
    ui->naturalScroll->setChecked(getBool("/labwc_config/libinput/device/naturalScroll"));

    /* Left Handed */
    settingsAddXmlBoo("/labwc_config/libinput/device/leftHanded", false);
    ui->leftHanded->setChecked(getBool("/labwc_config/libinput/device/leftHanded"));

    /* Pointer Speed */
    settingsAddXmlFlt("/labwc_config/libinput/device/pointerSpeed", 0.0f);
    ui->pointerSpeed->setValue(getFloat("/labwc_config/libinput/device/pointerSpeed"));

    /* Accel Profiles */
    settingsAddXmlStr("/labwc_config/libinput/device/accelProfile", "adaptive");
    QVector<QSharedPointer<Pair>> profiles;
    profiles.append(QSharedPointer<Pair>(new Pair("flat", tr("Flat"))));
    profiles.append(QSharedPointer<Pair>(new Pair("adaptive", tr("Adaptive"))));

    QString current_profile = getStr("/labwc_config/libinput/device/accelProfile");
    int profile_index = -1;
    foreach (auto profile, profiles) {
        ui->accelProfile->addItem(profile.get()->description(), QVariant(profile.get()->value()));
        ++profile_index;
        if (current_profile == profile.get()->value()) {
            ui->accelProfile->setCurrentIndex(profile_index);
        }
    }

    /* Tap to click */
    settingsAddXmlBoo("/labwc_config/libinput/device/tap", true);
    ui->tap->setChecked(getBool("/labwc_config/libinput/device/tap"));

    /* Tap Button Map */
    settingsAddXmlStr("/labwc_config/libinput/device/tapButtonMap", "lrm");
    QVector<QSharedPointer<Pair>> maps;
    maps.append(QSharedPointer<Pair>(new Pair("lrm", tr("left-right-middle"))));
    maps.append(QSharedPointer<Pair>(new Pair("lmr", tr("left-middle-right"))));

    QString current_map = getStr("/labwc_config/libinput/device/tapButtonMap");
    int map_index = -1;
    foreach (auto map, maps) {
        ui->tapButtonMap->addItem(map.get()->description(), QVariant(map.get()->value()));
        ++map_index;
        if (current_map == map.get()->value()) {
            ui->tapButtonMap->setCurrentIndex(map_index);
        }
    }

    /*
     * Tap And Drag
     *
     * Most devices have tap-and-drag enabled by default.
     *
     * Ref:
     *   - https://wayland.freedesktop.org/libinput/doc/latest/tapping.html#tapndrag
     */
    settingsAddXmlBoo("/labwc_config/libinput/device/tapAndDrag", true);
    ui->tapAndDrag->setChecked(getBool("/labwc_config/libinput/device/tapAndDrag"));

    /*
     * Drag Lock
     *
     * We disable this when tapAndDrag is unchecked.
     */
    settingsAddXmlBoo("/labwc_config/libinput/device/dragLock", false);
    ui->dragLock->setChecked(getBool("/labwc_config/libinput/device/dragLock"));
    ui->dragLock->setEnabled(ui->tapAndDrag->isChecked());
    ui->label_dragLock->setEnabled(ui->tapAndDrag->isChecked());
    connect(ui->tapAndDrag, &QCheckBox::toggled, ui->dragLock, &QWidget::setEnabled);
    connect(ui->tapAndDrag, &QCheckBox::toggled, ui->label_dragLock, &QWidget::setEnabled);

    /*
     * Three Finger Drag
     *
     * Ref:
     *   - https://wayland.freedesktop.org/libinput/doc/latest/drag-3fg.html#drag-3fg
     */
    settingsAddXmlBoo("/labwc_config/libinput/device/threeFingerDrag", false);
    ui->threeFingerDrag->setChecked(getBool("/labwc_config/libinput/device/threeFingerDrag"));

    /* Middle Emulation */
    settingsAddXmlBoo("/labwc_config/libinput/device/middleEmulation", false);
    ui->middleEmulation->setChecked(getBool("/labwc_config/libinput/device/middleEmulation"));

    /* Disable While Typing */
    settingsAddXmlBoo("/labwc_config/libinput/device/disableWhileTyping", true);
    ui->disableWhileTyping->setChecked(getBool("/labwc_config/libinput/device/disableWhileTyping"));

    /* Click Method */
    settingsAddXmlStr("/labwc_config/libinput/device/clickMethod", "none");
    QVector<QSharedPointer<Pair>> clickmethods;
    clickmethods.append(QSharedPointer<Pair>(new Pair("none", tr("None"))));
    clickmethods.append(QSharedPointer<Pair>(new Pair("buttonAreas", tr("Button Area"))));
    clickmethods.append(QSharedPointer<Pair>(new Pair("clickFinger", tr("Clickfinger"))));

    QString current_clickmethod = getStr("/labwc_config/libinput/device/clickMethod");
    int clickmethod_index = -1;
    foreach (auto clickmethod, clickmethods) {
        ui->clickMethod->addItem(clickmethod.get()->description(),
                                 QVariant(clickmethod.get()->value()));
        ++clickmethod_index;
        if (current_clickmethod == clickmethod.get()->value()) {
            ui->clickMethod->setCurrentIndex(clickmethod_index);
        }
    }

    /* Scroll Method */
    settingsAddXmlStr("/labwc_config/libinput/device/scrollMethod", "twoFinger");
    QVector<QSharedPointer<Pair>> scrollmethods;
    scrollmethods.append(QSharedPointer<Pair>(new Pair("twoFinger", tr("Two Finger"))));
    scrollmethods.append(QSharedPointer<Pair>(new Pair("edge", tr("Edge"))));
    scrollmethods.append(QSharedPointer<Pair>(new Pair("none", tr("None"))));

    QString current_scrollmethod = getStr("/labwc_config/libinput/device/scrollMethod");
    int scrollmethod_index = -1;
    foreach (auto scrollmethod, scrollmethods) {
        ui->scrollMethod->addItem(scrollmethod.get()->description(),
                                  QVariant(scrollmethod.get()->value()));
        ++scrollmethod_index;
        if (current_scrollmethod == scrollmethod.get()->value()) {
            ui->scrollMethod->setCurrentIndex(scrollmethod_index);
        }
    }

    /*
     * Send Events Mode
     *
     * Note: We cannot support 'No' until the device="" option is supported because otherwise all
     * devices (including keyboard) will be disabled which is unlikely to be the desired outcome.
     */
    settingsAddXmlStr("/labwc_config/libinput/device/sendEventsMode", "yes");
    QVector<QSharedPointer<Pair>> sendeventsmodes;
    sendeventsmodes.append(QSharedPointer<Pair>(new Pair("yes", tr("Enabled"))));
    sendeventsmodes.append(QSharedPointer<Pair>(
            new Pair("disabledOnExternalMouse", tr("Disable with external mouse"))));

    QString current_sendeventsmode = getStr("/labwc_config/libinput/device/sendEventsMode");
    int sendeventsmode_index = -1;
    foreach (auto sendeventsmode, sendeventsmodes) {
        ui->sendEventsMode->addItem(sendeventsmode.get()->description(),
                                    QVariant(sendeventsmode.get()->value()));
        ++sendeventsmode_index;
        if (current_sendeventsmode == sendeventsmode.get()->value()) {
            ui->sendEventsMode->setCurrentIndex(sendeventsmode_index);
        }
    }

    /* Scroll Factor */
    settingsAddXmlFlt("/labwc_config/libinput/device/scrollFactor", 1.0f);
    ui->scrollFactor->setValue(getFloat("/labwc_config/libinput/device/scrollFactor"));

    /* Touchscreen Rotation */
    settingsAddXmlStr("/labwc_config/libinput/device/calibrationMatrix", "");
    QVector<QSharedPointer<Pair>> calibrationmatrixes;
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("", tr(""))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("1 0 0 0 1 0", tr("Normal"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("0 -1 1 1 0 0", tr("Left"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("0 1 0 -1 0 1", tr("Right"))));
    calibrationmatrixes.append(QSharedPointer<Pair>(new Pair("-1 0 1 0 -1 1", tr("Inverted"))));

    QString current_calibrationmatrix = getStr("/labwc_config/libinput/device/calibrationMatrix");
    int calibrationmatrix_index = -1;
    foreach (auto calibrationmatrix, calibrationmatrixes) {
        ui->calibrationMatrix->addItem(calibrationmatrix.get()->description(),
                                  QVariant(calibrationmatrix.get()->value()));
        ++calibrationmatrix_index;
        if (current_calibrationmatrix == calibrationmatrix.get()->value()) {
            ui->calibrationMatrix->setCurrentIndex(calibrationmatrix_index);
        }
    }
}

void Mouse::onApply()
{
    /* ~/.config/labwc/rc.xml */
    setBool("/labwc_config/libinput/device/naturalScroll", ui->naturalScroll->isChecked());
    setBool("/labwc_config/libinput/device/leftHanded", ui->leftHanded->isChecked());
    setFloat("/labwc_config/libinput/device/pointerSpeed", ui->pointerSpeed->value() / 10.0);
    setStr("/labwc_config/libinput/device/accelProfile", DATA(ui->accelProfile));
    setBool("/labwc_config/libinput/device/tap", ui->tap->isChecked());
    setStr("/labwc_config/libinput/device/tapButtonMap", DATA(ui->tapButtonMap));
    setBool("/labwc_config/libinput/device/tapAndDrag", ui->tapAndDrag->isChecked());
    setBool("/labwc_config/libinput/device/dragLock", ui->dragLock->isChecked());
    setBool("/labwc_config/libinput/device/threeFingerDrag", ui->threeFingerDrag->isChecked());
    setBool("/labwc_config/libinput/device/middleEmulation", ui->middleEmulation->isChecked());
    setBool("/labwc_config/libinput/device/disableWhileTyping",
            ui->disableWhileTyping->isChecked());
    setStr("/labwc_config/libinput/device/clickMethod", DATA(ui->clickMethod));
    setStr("/labwc_config/libinput/device/scrollMethod", DATA(ui->scrollMethod));
    setStr("/labwc_config/libinput/device/sendEventsMode", DATA(ui->sendEventsMode));
    setFloat("/labwc_config/libinput/device/scrollFactor", ui->scrollFactor->value());
    setStr("/labwc_config/libinput/device/calibrationMatrix", DATA(ui->calibrationMatrix));

    /* ~/.config/labwc/environment */
    setStr("XCURSOR_THEME", TEXT(ui->cursorTheme));
    setInt("XCURSOR_SIZE", ui->cursorSize->value());
}
