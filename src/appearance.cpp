#include "appearance.h"
#include "find-themes.h"
#include "macros.h"
#include "settings.h"
#include "pair.h"
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
    /* Labwc Theme */
    QStringList labwcThemes = findLabwcThemes();
    ui->openboxTheme->addItems(labwcThemes);
    ui->openboxTheme->setCurrentIndex(labwcThemes.indexOf(getStr("/labwc_config/theme/name")));

    /* Corner Radius */
    ui->cornerRadius->setValue(getInt("/labwc_config/theme/cornerRadius"));
    ui->cornerRadius->setToolTip(tr("Radius of server side decoration top corners"));

    /* Drop Shadows */
    ui->dropShadows->setChecked(getBool("/labwc_config/theme/dropShadows"));
    ui->dropShadows->setToolTip(tr("Render drop-shadows behind windows"));

    /* Drop Shadows On Tiled */
    ui->dropShadowsOnTiled->setChecked(getBool("/labwc_config/theme/dropShadowsOnTiled"));
    ui->dropShadowsOnTiled->setToolTip(tr("Render drop-shadows behind tiled windows"));

    // Disable it when Drop Shadows is unchecked
    ui->dropShadowsOnTiled->setEnabled(ui->dropShadows->isChecked());
    connect(ui->dropShadows, &QCheckBox::toggled, ui->dropShadowsOnTiled, &QWidget::setEnabled);

    /* Icon Theme */
    QStringList themes = findIconThemes(LAB_ICON_THEME_TYPE_ICON);
    ui->iconTheme->addItems(themes);
    ui->iconTheme->setCurrentIndex(themes.indexOf(getStr("/labwc_config/theme/icon")));

    /* Keep Border */
    ui->keepBorder->setChecked(getBool("/labwc_config/theme/keepBorder"));
    ui->keepBorder->setToolTip(tr("Even when disabling server side decorations via ToggleDecorations,\nkeep a small border (and resize area) around the window"));

    /* Mmaximized Decoration */
    ui->maximizedDecoration->clear(); // remove 2 empty values created for some reason
    ui->maximizedDecoration->setToolTip(tr("Specify how server side decorations are shown for maximized windows"));

    QVector<QSharedPointer<Pair>> decorations;
    decorations.append(QSharedPointer<Pair>(new Pair("titlebar", tr("Titlebar"))));
    decorations.append(QSharedPointer<Pair>(new Pair("none", tr("None"))));

    QString current_decoration = getStr("/labwc_config/theme/maximizedDecoration");
    int decoration_index = -1;
    foreach (auto decoration, decorations) {
        ui->maximizedDecoration->addItem(decoration.get()->description(), QVariant(decoration.get()->value()));
        ++decoration_index;
        if (current_decoration == decoration.get()->value()) {
            ui->maximizedDecoration->setCurrentIndex(decoration_index);
        }
    }

}

void Appearance::onApply()
{
    setInt("/labwc_config/theme/cornerRadius", ui->cornerRadius->value());
    setStr("/labwc_config/theme/name", TEXT(ui->openboxTheme));
    setBool("/labwc_config/theme/dropShadows", ui->dropShadows->isChecked());
    setBool("/labwc_config/theme/dropShadowsOnTiled", ui->dropShadowsOnTiled->isChecked());
    setStr("/labwc_config/theme/icon", TEXT(ui->iconTheme));
    setBool("/labwc_config/theme/keepBorder", ui->keepBorder->isChecked());
    setStr("/labwc_config/theme/maximizedDecoration", DATA(ui->maximizedDecoration));
}
