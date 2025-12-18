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
    QStringList cursorThemes = findIconThemes(LAB_ICON_THEME_TYPE_CURSOR);
    ui->cursorTheme->addItems(cursorThemes);
    ui->cursorTheme->setCurrentIndex(cursorThemes.indexOf(getStr("XCURSOR_THEME")));

    /* Cursor Size */
    ui->cursorSize->setValue(getInt("XCURSOR_SIZE"));

    /* Natural Scroll */
    ui->naturalScroll->setChecked(getBool("/labwc_config/libinput/device/naturalScroll"));

    /* Left Handed */
    ui->leftHanded->setChecked(getBool("/labwc_config/libinput/device/leftHanded"));

    /* Pointer Speed */
    ui->pointerSpeed->setValue(getFloat("/labwc_config/libinput/device/pointerSpeed"));

    /* Accel Profiles */
    ui->accelProfile->clear(); // remove 2 empty values created for some reason

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
    ui->tap->setChecked(getBool("/labwc_config/libinput/device/tap"));

    /* Tap Button Map */
    ui->tapButtonMap->clear(); // remove 2 empty values created for some reason

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

    /* Tap And Drag */
    ui->tapAndDrag->setChecked(getBool("/labwc_config/libinput/device/tapAndDrag"));

    /* Drag Lock */
    ui->dragLock->setChecked(getBool("/labwc_config/libinput/device/dragLock"));

    // Disable it when tapAndDrag is unchecked
    ui->dragLock->setEnabled(ui->tapAndDrag->isChecked());
    ui->label_dragLock->setEnabled(ui->tapAndDrag->isChecked());

    connect(ui->tapAndDrag, &QCheckBox::toggled, ui->dragLock, &QWidget::setEnabled);
    connect(ui->tapAndDrag, &QCheckBox::toggled, ui->label_dragLock, &QWidget::setEnabled);

    /* 3 Finger Drag */
    ui->threeFingerDrag->setChecked(getBool("/labwc_config/libinput/device/threeFingerDrag"));

    /* Middle Emulation */
    ui->middleEmulation->setChecked(getBool("/labwc_config/libinput/device/middleEmulation"));

    /* Disable While Typing */
    ui->disableWhileTyping->setChecked(getBool("/labwc_config/libinput/device/disableWhileTyping"));

    /* Click Method */
    ui->clickMethod->clear();

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
    ui->scrollMethod->clear();

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

    /* Send Events Mode */
    ui->sendEventsMode->clear();

    // Note: Cannot support 'No' until the device="" option is supported because otherwise all
    // devices (including keyboard) will be disabled which is unlikely to be the desired outcome.
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
    ui->scrollFactor->setValue(getFloat("/labwc_config/libinput/device/scrollFactor"));
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

    /* ~/.config/labwc/environment */
    environmentSet("XCURSOR_THEME", TEXT(ui->cursorTheme));
    environmentSetInt("XCURSOR_SIZE", ui->cursorSize->value());
}
