#include "mouse.h"
#include "environment.h"
#include "find-themes.h"
#include "macros.h"
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
    ui->naturalScroll->addItem("no");
    ui->naturalScroll->addItem("yes");
    ui->naturalScroll->setCurrentIndex(getBool("/labwc_config/libinput/device/naturalScroll"));
}

void Mouse::onApply()
{
    /* ~/.config/labwc/rc.xml */
    setBoolfromString("/labwc_config/libinput/device/naturalScroll", TEXT(ui->naturalScroll));

    /* ~/.config/labwc/environment */
    environmentSet("XCURSOR_THEME", TEXT(ui->cursorTheme));
    environmentSetInt("XCURSOR_SIZE", ui->cursorSize->value());
}
