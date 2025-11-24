#include "language.h"
#include "evdev-lst-layouts.h"
#include "environment.h"
#include "find-themes.h"
#include "layoutmodel.h"
#include "macros.h"
#include "settings.h"
#include "./ui_language.h"

Language::Language(QWidget *parent) : QWidget(parent), ui(new Ui::pageLanguage)
{
    ui->setupUi(this);

    m_model = new LayoutModel(this);
    ui->layoutView->setModel(m_model);
    connect(ui->layoutAdd, &QPushButton::pressed, this, &Language::addSelectedLayout);
    connect(ui->layoutRemove, &QPushButton::pressed, this, &Language::deleteSelectedLayout);
}

Language::~Language()
{
    delete ui;
}

void Language::activate()
{
    /* Keyboard Layout */
    ui->layoutCombo->addItem(tr("Select layout to add..."));
    for (auto layout : evdev_lst_layouts) {
        ui->layoutCombo->addItem(layout.description);
    }
}

void Language::onApply()
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
}

void Language::addSelectedLayout(void)
{
    QString description = ui->layoutCombo->currentText();

    for (auto layout : evdev_lst_layouts) {
        if (description == layout.description) {
            m_model->addLayout(layout.code, layout.description);
        }
    }
}

void Language::deleteSelectedLayout(void)
{
    m_model->deleteLayout(ui->layoutView->currentIndex().row());
}

