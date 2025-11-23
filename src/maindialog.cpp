#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStandardPaths>
#include <QTextStream>
#include <string>
#include <unistd.h>
#include "environment.h"
#include "evdev-lst-layouts.h"
#include "find-themes.h"
#include "layoutmodel.h"
#include "log.h"
#include "macros.h"
#include "maindialog.h"
#include "./ui_maindialog.h"
#include "xml.h"

MainDialog::MainDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    ui->list->setFixedWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());

    m_model = new LayoutModel(this);
    ui->layoutView->setModel(m_model);

    QObject::connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton *button) {
        if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply) {
            onApply();
        }
    });

    connect(ui->layoutAdd, &QPushButton::pressed, this, &MainDialog::addSelectedLayout);
    connect(ui->layoutRemove, &QPushButton::pressed, this, &MainDialog::deleteSelectedLayout);

    activate();
}

MainDialog::~MainDialog()
{
    delete ui;
    xml_finish();
}

void MainDialog::addSelectedLayout(void)
{
    const char *description = ui->layoutCombo->currentText().toLatin1().data();
    for (auto layout : evdev_lst_layouts) {
        if (!strcmp(description, layout.description)) {
            m_model->addLayout(QString(layout.code), QString(layout.description));
        }
    }
}

void MainDialog::deleteSelectedLayout(void)
{
    m_model->deleteLayout(ui->layoutView->currentIndex().row());
}

void MainDialog::activate()
{
    ui->pageAppearance->activate();
    ui->pageBehaviour->activate();
    ui->pageMouse->activate();

    /* # LANGUAGE */

    /* Keyboard Layout */
    ui->layoutCombo->addItem(tr("Select layout to add..."));
    for (auto layout : evdev_lst_layouts) {
        ui->layoutCombo->addItem(QString(layout.description));
    }
}

void MainDialog::onApply()
{
    ui->pageAppearance->onApply();
    ui->pageBehaviour->onApply();
    ui->pageMouse->onApply();

    /*
     * We include variants in XKB_DEFAULT_LAYOUT, for example "latam(deadtilde),ru(phonetic),gr",
     * so XKB_DEFAULT_VARIANT is set to empty.
     */
    QString layout = m_model->getXkbDefaultLayout();
    if (!layout.isEmpty()) {
        environmentSet("XKB_DEFAULT_LAYOUT", layout);
        environmentSet("XKB_DEFAULT_VARIANT", "");
    }

    // TODO: Get filename in a more consistent way - share common code with main.cpp
    std::string config_home = std::getenv("HOME") + std::string("/.config/labwc");
    std::string config_dir = std::getenv("LABWC_CONFIG_DIR") ?: config_home;
    std::string environment_file = config_dir + "/environment";

    xml_save();
    environmentSave(environment_file);

    /* reconfigure labwc */
    if (!fork()) {
        execl("/bin/sh", "/bin/sh", "-c", "labwc -r", (void *)NULL);
    }
}
