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
    settingsAddXmlStr("/labwc_config/theme/name", "");
    QStringList labwcThemes = findLabwcThemes();
    ui->openboxTheme->addItems(labwcThemes);
    ui->openboxTheme->setCurrentIndex(labwcThemes.indexOf(getStr("/labwc_config/theme/name")));

    /* Corner Radius */
    settingsAddXmlInt("/labwc_config/theme/cornerRadius", 8);
    ui->cornerRadius->setValue(getInt("/labwc_config/theme/cornerRadius"));
    ui->cornerRadius->setToolTip(tr("Radius of server side decoration top corners"));

    /* Drop Shadows */
    settingsAddXmlBoo("/labwc_config/theme/dropShadows", false);
    ui->dropShadows->setChecked(getBool("/labwc_config/theme/dropShadows"));
    ui->dropShadows->setToolTip(tr("Render drop-shadows behind windows"));

    /* Drop Shadows On Tiled */
    settingsAddXmlBoo("/labwc_config/theme/dropShadowsOnTiled", false);
    ui->dropShadowsOnTiled->setChecked(getBool("/labwc_config/theme/dropShadowsOnTiled"));
    ui->dropShadowsOnTiled->setToolTip(tr("Render drop-shadows behind tiled windows"));

    // Disable it when Drop Shadows is unchecked
    ui->dropShadowsOnTiled->setEnabled(ui->dropShadows->isChecked());
    connect(ui->dropShadows, &QCheckBox::toggled, ui->dropShadowsOnTiled, &QWidget::setEnabled);

    /* Icon Theme */
    settingsAddXmlStr("/labwc_config/theme/icon", "");
    QStringList themes = findIconThemes(LAB_ICON_THEME_TYPE_ICON);
    ui->iconTheme->addItems(themes);
    ui->iconTheme->setCurrentIndex(themes.indexOf(getStr("/labwc_config/theme/icon")));

    /* Decoration */
    settingsAddXmlStr("/labwc_config/core/decoration", "server");
    ui->decoration->setToolTip(tr("Specify decorations for xdg-shell windows"));

    QVector<QSharedPointer<Pair>> decorations;
    decorations.append(
            QSharedPointer<Pair>(new Pair("server", tr("Server Side Decoration (SSD)"))));
    decorations.append(
            QSharedPointer<Pair>(new Pair("client", tr("Client Side Decoration (CSD)"))));

    QString current_decoration = getStr("/labwc_config/core/decoration");
    int decoration_index = -1;
    foreach (auto decoration, decorations) {
        ui->decoration->addItem(decoration.get()->description(),
                                QVariant(decoration.get()->value()));
        ++decoration_index;
        if (current_decoration == decoration.get()->value()) {
            ui->decoration->setCurrentIndex(decoration_index);
        }
    }

    /* Maximized Decoration */
    settingsAddXmlStr("/labwc_config/theme/maximizedDecoration", "titlebar");
    ui->maximizedDecoration->setToolTip(tr("Show server side decorations on maximized windows"));

    QVector<QSharedPointer<Pair>> maximized_decorations;
    maximized_decorations.append(QSharedPointer<Pair>(new Pair("titlebar", tr("Titlebar"))));
    maximized_decorations.append(QSharedPointer<Pair>(new Pair("none", tr("None"))));

    QString current_maximized_decoration = getStr("/labwc_config/theme/maximizedDecoration");
    int maximized_decoration_index = -1;
    foreach (auto maximized_decoration, maximized_decorations) {
        ui->maximizedDecoration->addItem(maximized_decoration.get()->description(),
                                         QVariant(maximized_decoration.get()->value()));
        ++maximized_decoration_index;
        if (current_maximized_decoration == maximized_decoration.get()->value()) {
            ui->maximizedDecoration->setCurrentIndex(maximized_decoration_index);
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
    setStr("/labwc_config/core/decoration", DATA(ui->decoration));
    setStr("/labwc_config/theme/maximizedDecoration", DATA(ui->maximizedDecoration));
}
