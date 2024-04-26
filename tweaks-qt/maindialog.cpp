#include <QDebug>
#include <glib.h>
#include <string>
#include <unistd.h>
#include "maindialog.h"
#include "./ui_maindialog.h"

extern "C" {
#include "environment.h"
#include "keyboard-layouts.h"
#include "theme.h"
#include "xml.h"
}

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    ui->list->setFixedWidth(ui->list->sizeHintForColumn(0) + 2 * ui->list->frameWidth());

    std::string config_file = std::getenv("HOME");
    config_file += "/.config/labwc/rc.xml";
    xml_init(config_file.data());
    xml_setup_nodes();

    QObject::connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &MainDialog::onApply);

    activate();
}

MainDialog::~MainDialog()
{
    delete ui;
    xml_finish();
}

static const char *first_field(char *s, char delim)
{
    char *p = strchr(s, delim);
    if (p) {
        *p = '\0';
    }
    return s;
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
    GList *keyboard_layouts = NULL;
    keyboard_layouts_init(&keyboard_layouts, "/usr/share/X11/xkb/rules/evdev.lst");
    char xkb_default_layout[1024];
    environment_get(xkb_default_layout, sizeof(xkb_default_layout), "XKB_DEFAULT_LAYOUT");
    active = -1;

    GList *iter;
    int i = 0;
    for (iter = keyboard_layouts; iter; iter = iter->next) {
        struct layout *layout = (struct layout *)iter->data;
        if (!strcmp(layout->lang, first_field(xkb_default_layout, ','))) {
            active = i;
        }
        char buf[256];
        snprintf(buf, sizeof(buf), "%s  %s", layout->lang, layout->description);
        ui->keyboardLayout->addItem(buf);
        ++i;
    }
    if (active != -1) {
        ui->keyboardLayout->setCurrentIndex(active);
    }
    keyboard_layouts_finish(keyboard_layouts);
}

void MainDialog::onApply()
{
    /* ~/.config/labwc/rc.xml */
    xml_set_num("/labwc_config/theme/cornerradius", ui->cornerRadius->value());
    xml_set("/labwc_config/theme/name", ui->openboxTheme->currentText().toLatin1().data());
    xml_set("/labwc_config/libinput/device/naturalscroll",
            ui->naturalScroll->currentText().toLatin1().data());
    xml_save();

    /* ~/.config/labwc/environment */
    environment_set("XCURSOR_THEME", ui->cursorTheme->currentText().toLatin1().data());
    environment_set_num("XCURSOR_SIZE", ui->cursorSize->value());
    environment_set("XKB_DEFAULT_LAYOUT",
                    first_field(ui->keyboardLayout->currentText().toLatin1().data(), ' '));

    /* reconfigure labwc */
    if (!fork()) {
        execl("/bin/sh", "/bin/sh", "-c", "killall -s SIGHUP labwc", (void *)NULL);
    }
}
