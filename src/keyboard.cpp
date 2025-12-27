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

    /* Keyboard Layout Group Switching */
    ui->layoutGrpSwitcher->setToolTip(tr("Key combination to switch keyboard layout"));
    QVector<QSharedPointer<Pair>> combo;
    combo.append(QSharedPointer<Pair>(new Pair("", "")));
    combo.append(QSharedPointer<Pair>(new Pair("grp:shift_caps_toggle", tr("Shift+Caps Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:alt_caps_toggle", tr("Alt+Caps Lock"))));
    combo.append(QSharedPointer<Pair>(new Pair("grp:shifts_toggle", tr("Both Shifts together"))));

    QString current = getStr("XKB_DEFAULT_OPTIONS");
    int index = -1;
    foreach (auto policy, combo) {
        ui->layoutGrpSwitcher->addItem(policy.get()->description(), QVariant(policy.get()->value()));
        ++index;
        if (current == policy.get()->value()) {
            ui->layoutGrpSwitcher->setCurrentIndex(index);
        }
    }
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
        environmentSet("XKB_DEFAULT_LAYOUT", layout);
        environmentSet("XKB_DEFAULT_VARIANT", "");
    }

    setInt("/labwc_config/keyboard/repeatRate", ui->repeatRate->value());
    setInt("/labwc_config/keyboard/repeatDelay", ui->repeatDelay->value());
    setBool("/labwc_config/keyboard/numlock", ui->numlock->isChecked());

    environmentSet("XKB_DEFAULT_OPTIONS", DATA(ui->layoutGrpSwitcher));
}
