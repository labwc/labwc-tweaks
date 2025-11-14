#include "appearance.h"
#include "find-themes.h"
#include "macros.h"
#include "settings.h"
#include "./ui_appearance.h"

Appearance::Appearance(QWidget *parent) : QWidget(parent), ui(new Ui::pageAppearance)
{
    ui->setupUi(this);
}

Appearance::~Appearance()
{
    delete ui;
}

void Appearance::activate()
{
    /* # APPEARANCE */

    /* Labwc Theme */
    QStringList labwcThemes = findLabwcThemes();
    ui->openboxTheme->addItems(labwcThemes);
    ui->openboxTheme->setCurrentIndex(labwcThemes.indexOf(getStr("/labwc_config/theme/name")));

    /* Corner Radius */
    ui->cornerRadius->setValue(getInt("/labwc_config/theme/cornerRadius"));

    /* Drop Shadows */
    ui->dropShadows->addItem("no");
    ui->dropShadows->addItem("yes");
    ui->dropShadows->setCurrentIndex(getBool("/labwc_config/theme/dropShadows"));

    /* Icon Theme */
    QStringList themes = findIconThemes(LAB_ICON_THEME_TYPE_ICON);
    ui->iconTheme->addItems(themes);
    ui->iconTheme->setCurrentIndex(themes.indexOf(getStr("/labwc_config/theme/icon")));
}

void Appearance::onApply()
{
    setInt("/labwc_config/theme/cornerRadius", ui->cornerRadius->value());
    setStr("/labwc_config/theme/name", TEXT(ui->openboxTheme));
    setBool("/labwc_config/theme/dropShadows", TEXT(ui->dropShadows));
    setStr("/labwc_config/theme/icon", TEXT(ui->iconTheme));
}
