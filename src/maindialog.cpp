#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStandardPaths>
#include <QTextStream>
#include <string>
#include <unistd.h>
#include "environment.h"
#include "find-themes.h"
#include "log.h"
#include "macros.h"
#include "maindialog.h"
#include "./ui_maindialog.h"
#include "xml.h"

MainDialog::MainDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    ui->list->setFixedWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());
    QObject::connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton *button) {
        if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply) {
            onApply();
        }
    });
    activate();
}

MainDialog::~MainDialog()
{
    delete ui;
    xml_finish();
}

void MainDialog::activate()
{
    ui->pageAppearance->activate();
    ui->pageBehaviour->activate();
    ui->pageMouse->activate();
    ui->pageLanguage->activate();
}

void MainDialog::onApply()
{
    ui->pageAppearance->onApply();
    ui->pageBehaviour->onApply();
    ui->pageMouse->onApply();
    ui->pageLanguage->onApply();

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
