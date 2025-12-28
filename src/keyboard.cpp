#include "keyboard.h"
#include "evdev-lst-layouts.h"
#include "environment.h"
#include "find-themes.h"
#include "layoutmodel.h"
#include "macros.h"
#include "settings.h"
#include "./ui_keyboard.h"

Keyboard::Keyboard(QWidget *parent) : QWidget(parent), ui(new Ui::pageKeyboard)
{
    ui->setupUi(this);

    m_model = new LayoutModel(this);
    ui->layoutView->setModel(m_model);
    connect(ui->layoutAdd, &QPushButton::pressed, this, &Keyboard::addSelectedLayout);
    connect(ui->layoutRemove, &QPushButton::pressed, this, &Keyboard::deleteSelectedLayout);
}

Keyboard::~Keyboard()
{
    delete ui;
}

void Keyboard::getGrpToggleOptions(QVector<QSharedPointer<Pair>> &combo)
{
    // Generated based on XKEYBOARD-CONFIG(7)
    // clang-format off
    combo.append(QSharedPointer<Pair>(new Pair("", "")));
    combo.append(QSharedPointer<Pair>(new Pair("grp:shift_caps_toggle", tr("Shift+Caps Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_caps_toggle", tr("Alt+Caps Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:shifts_toggle", tr("Both Shifts together"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alts_toggle", tr("Both Alts together"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrls_toggle", tr("Both Ctrls together"))));
    // --- separator after 6 items ---
    combo.append(QSharedPointer<Pair>(new Pair("grp:switch", tr("Right Alt (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lswitch", tr("Left Alt (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lwin_switch", tr("Left Win (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rwin_switch", tr("Right Win (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:win_switch", tr("Any Win (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:menu_switch",
        tr("Menu (while pressed), Shift+Menu for Menu"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:caps_switch",
        tr("Caps Lock (while pressed), Alt+Caps Lock for the original Caps Lock action"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rctrl_switch",
        tr("Right Ctrl (while pressed)"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:toggle", tr("Right Alt"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lalt_toggle", tr("Left Alt"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:caps_toggle", tr("Caps Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:caps_select",
        tr("Caps Lock to first layout; Shift+Caps Lock to second layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:win_menu_select",
        tr("Left Win to first layout; Right Win/Menu to second layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_select",
        tr("Left Ctrl to first layout; Right Ctrl to second layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_altgr_toggle",
        tr("Both Alts together; AltGr alone chooses third level"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_shift_toggle", tr("Ctrl+Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lctrl_lshift_toggle",
        tr("Left Ctrl+Left Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rctrl_rshift_toggle",
        tr("Right Ctrl+Right Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_shift_toggle_bidir",
        tr("Left Ctrl+Left Shift chooses previous layout, Right Ctrl + Right Shift chooses next layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_alt_toggle", tr("Alt+Ctrl"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lctrl_lalt_toggle", tr("Left Alt+Left Ctrl"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rctrl_ralt_toggle",
        tr("Right Alt+Right Ctrl"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_alt_toggle_bidir",
        tr("Left Ctrl+Left Alt chooses previous layout, Right Ctrl + Right Alt chooses next layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_shift_toggle", tr("Alt+Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lalt_lshift_toggle",
        tr("Left Alt+Left Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ralt_rshift_toggle",
        tr("Right Alt+Right Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_shift_toggle_bidir",
        tr("Left Alt+Left Shift chooses previous layout, Right Alt + Right Shift chooses next layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:menu_toggle", tr("Menu"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lwin_toggle", tr("Left Win"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_space_toggle", tr("Alt+Space"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:win_space_toggle", tr("Win+Space"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:ctrl_space_toggle", tr("Ctrl+Space"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rwin_toggle", tr("Right Win"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lshift_toggle", tr("Left Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rshift_toggle", tr("Right Shift"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lctrl_toggle", tr("Left Ctrl"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:rctrl_toggle", tr("Right Ctrl"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:sclk_toggle", tr("Scroll Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lctrl_lwin_rctrl_menu",
        tr("Ctrl+Left Win to first layout; Ctrl+Menu to second layout"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:lctrl_lwin_toggle", tr("Left Ctrl+Left Win"))));
    // clang-format on
}

void Keyboard::activate()
{
    /* Keyboard Layout */
    ui->layoutCombo->addItem(tr("Select layout to add..."));
    for (auto layout : evdev_lst_layouts) {
        ui->layoutCombo->addItem(layout.description);
    }

    /* Repeat Rate */
    ui->repeatRate->setValue(getInt("/labwc_config/keyboard/repeatRate"));
    ui->repeatRate->setToolTip(tr("Rate at which keypresses are repeated per second"));

    /* Repeat Delay */
    ui->repeatDelay->setValue(getInt("/labwc_config/keyboard/repeatDelay"));
    ui->repeatDelay->setToolTip(tr("Delay before keypresses are repeated"));

    /* Numlock */
    ui->numlock->setChecked(getBool("/labwc_config/keyboard/numlock"));
    ui->numlock->setToolTip(tr("Enable Num Lock when recognizing a new keyboard"));

    // Keyboard Layout Group Switching
    ui->layoutGrpSwitcher->setToolTip(tr("Key combination to switch keyboard layout"));
    QVector<QSharedPointer<Pair>> combo;
    getGrpToggleOptions(combo);
    QString current = getStr("XKB_DEFAULT_OPTIONS");
    int index = -1;
    foreach (auto policy, combo) {
        ui->layoutGrpSwitcher->addItem(policy.get()->description(), QVariant(policy.get()->value()));
        ++index;
        if (current == policy.get()->value()) {
            ui->layoutGrpSwitcher->setCurrentIndex(index);
        }
    }
    ui->layoutGrpSwitcher->insertSeparator(6);
}

void Keyboard::addSelectedLayout(void)
{
    QString description = ui->layoutCombo->currentText();

    for (auto layout : evdev_lst_layouts) {
        if (description == layout.description) {
            m_model->addLayout(layout.code, layout.description);
        }
    }
}

void Keyboard::deleteSelectedLayout(void)
{
    m_model->deleteLayout(ui->layoutView->currentIndex().row());
}

void Keyboard::onApply()
{
    /*
     * We include variants in XKB_DEFAULT_LAYOUT, for example
     * "latam(deadtilde),ru(phonetic),gr", so XKB_DEFAULT_VARIANT is set to
     * empty.
     */
    QString layout = m_model->getXkbDefaultLayout();
    if (!layout.isEmpty()) {
        setStr("XKB_DEFAULT_LAYOUT", layout);
        environmentSet("XKB_DEFAULT_VARIANT", "");
    }

    setInt("/labwc_config/keyboard/repeatRate", ui->repeatRate->value());
    setInt("/labwc_config/keyboard/repeatDelay", ui->repeatDelay->value());
    setBool("/labwc_config/keyboard/numlock", ui->numlock->isChecked());

    setStr("XKB_DEFAULT_OPTIONS", DATA(ui->layoutGrpSwitcher));
}
