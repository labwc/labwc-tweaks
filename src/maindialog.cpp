#include <QDebug>
#include <string>
#include <unistd.h>
#include "environment.h"
#include "evdev-lst-layouts.h"
#include "layoutmodel.h"
#include "maindialog.h"
#include "./ui_maindialog.h"

extern "C" {
#include "theme.h"
#include "xml.h"
}

MainDialog::MainDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    ui->list->setFixedWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());

    m_model = new LayoutModel(this);
    ui->layoutView->setModel(m_model);

    std::string config_dir =
            std::getenv("LABWC_CONFIG_DIR") ?: std::getenv("HOME") + std::string("/.config/labwc");
    std::string config_file = config_dir + "/rc.xml";
    initConfig(config_file);

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
    /* # APPEARANCE */

    /* Openbox Theme */
    struct themes openbox_themes = { 0 };
    theme_find(&openbox_themes, "themes", "openbox-3/themerc");
    int active = -1;
    const char *active_id = xml_get("/labwc_config/theme/name");
    for (int i = 0; i < openbox_themes.nr; ++i) {
        struct theme *theme = openbox_themes.data + i;
        if (active_id && !strcmp(theme->name, active_id)) {
            active = i;
        }
        ui->openboxTheme->addItem(theme->name);
    }
    if (active != -1) {
        ui->openboxTheme->setCurrentIndex(active);
    }
    theme_free_vector(&openbox_themes);

    /* Corner Radius */
    ui->cornerRadius->setValue(xml_get_int("/labwc_config/theme/cornerradius"));

    /* Drop Shadows */
    ui->dropShadows->addItem("no");
    ui->dropShadows->addItem("yes");
    ui->dropShadows->setCurrentIndex(xml_get_bool_text("/labwc_config/theme/dropShadows"));

    /* # BEHAVIOUR */
    std::vector policies = { "", "Center", "Automatic", "Cursor" };
    active = -1;
    active_id = xml_get("/labwc_config/placement/policy");
    int i = 0;
    for (auto policy : policies) {
        if (active_id && !strcasecmp(policy, active_id)) {
            active = i;
        }
        ui->placementPolicy->addItem(policy);
        ++i;
    }
    if (active != -1) {
        ui->placementPolicy->setCurrentIndex(active);
    }

    /* # MOUSE & TOUCHPAD */

    /* Cursor Theme */
    struct themes cursor_themes = { 0 };
    theme_find(&cursor_themes, "icons", "cursors");

    active_id = getenv("XCURSOR_THEME") ?: (char *)"";
    active = -1;
    for (int i = 0; i < cursor_themes.nr; ++i) {
        struct theme *theme = cursor_themes.data + i;
        if (!strcmp(theme->name, active_id)) {
            active = i;
        }
        ui->cursorTheme->addItem(theme->name);
    }
    if (active != -1) {
        ui->cursorTheme->setCurrentIndex(active);
    }
    theme_free_vector(&cursor_themes);

    /* Cursor Size */
    ui->cursorSize->setValue(atoi(getenv("XCURSOR_SIZE") ?: "24"));

    /* Natural Scroll */
    ui->naturalScroll->addItem("no");
    ui->naturalScroll->addItem("yes");
    ui->naturalScroll->setCurrentIndex(
            xml_get_bool_text("/labwc_config/libinput/device/naturalscroll"));

    /* # LANGUAGE */

    /* Keyboard Layout */
    ui->layoutCombo->addItem(tr("Select layout to add..."));
    for (auto layout : evdev_lst_layouts) {
        ui->layoutCombo->addItem(QString(layout.description));
    }
}

void MainDialog::initConfig(std::string &config_file)
{
    xml_init(config_file.data());

    /* Ensure all relevant nodes exist before we start getting/setting */
    xpath_add_node("/labwc_config/theme/cornerRadius");
    xpath_add_node("/labwc_config/theme/name");
    xpath_add_node("/labwc_config/theme/dropShadows");
    xpath_add_node("/labwc_config/placement/policy");
    xpath_add_node("/labwc_config/libinput/device/naturalScroll");

    xml_save();
}

void MainDialog::onApply()
{
    /* ~/.config/labwc/rc.xml */
    xml_set_num("/labwc_config/theme/cornerradius", ui->cornerRadius->value());
    xml_set("/labwc_config/theme/name", ui->openboxTheme->currentText().toLatin1().data());
    xml_set("/labwc_config/theme/dropShadows", ui->dropShadows->currentText().toLatin1().data());
    xml_set("/labwc_config/libinput/device/naturalscroll",
            ui->naturalScroll->currentText().toLatin1().data());
    xml_set("/labwc_config/placement/policy", ui->placementPolicy->currentText().toLatin1().data());
    xml_save();

    /* ~/.config/labwc/environment */
    environment_set("XCURSOR_THEME", ui->cursorTheme->currentText().toLatin1().data());
    environment_set_num("XCURSOR_SIZE", ui->cursorSize->value());

    /*
     * We include variants in XKB_DEFAULT_LAYOUT, for example "latam(deadtilde),ru(phonetic),gr",
     * so XKB_DEFAULT_VARIANT is set to empty.
     */
    const char *layout = m_model->getXkbDefaultLayout();
    if (layout && *layout) {
        environment_set("XKB_DEFAULT_LAYOUT", layout);
        environment_set("XKB_DEFAULT_VARIANT", "");
    }

    /* reconfigure labwc */
    if (!fork()) {
        execl("/bin/sh", "/bin/sh", "-c", "labwc -r", (void *)NULL);
    }
}
