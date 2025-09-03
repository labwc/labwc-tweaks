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
#include "layoutmodel.h"
#include "log.h"
#include "maindialog.h"
#include "settings.h"
#include "./ui_maindialog.h"

extern "C" {
#include "theme.h"
#include "xml.h"
}

MainDialog::MainDialog(std::vector<std::shared_ptr<Setting>> &settings, QWidget *parent)
    : QDialog(parent), ui(new Ui::MainDialog), m_settings(settings)
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
    /* # APPEARANCE */

    // TODO: Use retrieve() instead of xml_get(), etc.

    /* Labwc Theme */
    QStringList labwcThemes = findLabwcThemes();
    ui->openboxTheme->addItems(labwcThemes);
    ui->openboxTheme->setCurrentIndex(labwcThemes.indexOf(xml_get("/labwc_config/theme/name")));

    /* Corner Radius */
    ui->cornerRadius->setValue(xml_get_int("/labwc_config/theme/cornerRadius"));

    /* Drop Shadows */
    ui->dropShadows->addItem("no");
    ui->dropShadows->addItem("yes");
    ui->dropShadows->setCurrentIndex(xml_get_bool_text("/labwc_config/theme/dropShadows"));

    /* Icon Theme */
    QStringList themes = findIconThemes(LAB_ICON_THEME_TYPE_ICON);
    ui->iconTheme->addItems(themes);
    ui->iconTheme->setCurrentIndex(themes.indexOf(xml_get("/labwc_config/theme/icon")));

    /* # BEHAVIOUR */
    std::vector policies = { "", "Automatic", "Cascade", "Center", "Cursor" };
    int active = -1;
    const char *active_id = xml_get("/labwc_config/placement/policy");
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
    QStringList cursorThemes = findIconThemes(LAB_ICON_THEME_TYPE_CURSOR);
    ui->cursorTheme->addItems(cursorThemes);
    ui->cursorTheme->setCurrentIndex(cursorThemes.indexOf(getenv("XCURSOR_THEME") ?: (char *)""));

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

void setInt(std::vector<std::shared_ptr<Setting>> &settings, QString name, int value)
{
    std::shared_ptr<Setting> setting = retrieve(settings, name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_INT) {
        qDebug() << "setInt(): not valid int setting" << name << value;
    }
    if (value != std::get<int>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value);
        xml_set_num(name.toStdString().c_str(), value);
    }
}

void setStr(std::vector<std::shared_ptr<Setting>> &settings, QString name, QString value)
{
    std::shared_ptr<Setting> setting = retrieve(settings, name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_STRING) {
        qDebug() << "setStr(): not valid string setting" << name << value;
    }
    if (value != std::get<QString>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value.toStdString());
        xml_set(name.toStdString().c_str(), value.toStdString().c_str());
    }
}

/**
 * parse_bool() - Parse boolean value of string.
 * @string: String to interpret. This check is case-insensitive.
 * @default_value: Default value to use if string is not a recognised boolean.
 *                 Use -1 to avoid setting a default value.
 *
 * Return: 0 for false; 1 for true; -1 for non-boolean
 */
int parseBool(const char *str, int defaultValue)
{
    if (!str)
        goto error_not_a_boolean;
    else if (!strcasecmp(str, "yes"))
        return 1;
    else if (!strcasecmp(str, "true"))
        return 1;
    else if (!strcasecmp(str, "on"))
        return 1;
    else if (!strcmp(str, "1"))
        return 1;
    else if (!strcasecmp(str, "no"))
        return 0;
    else if (!strcasecmp(str, "false"))
        return 0;
    else if (!strcasecmp(str, "off"))
        return 0;
    else if (!strcmp(str, "0"))
        return 0;
error_not_a_boolean:
    qDebug() << str << "is not a boolean value";
    return defaultValue;
}

// TODO: make this more bool-ish
void setBool(std::vector<std::shared_ptr<Setting>> &settings, QString name, QString value)
{
    std::shared_ptr<Setting> setting = retrieve(settings, name);
    if (setting == nullptr) {
        qDebug() << "warning: no settings with name" << name;
        return;
    }
    if (setting->valueType() != LAB_VALUE_TYPE_BOOL) {
        qDebug() << "setBool(): not valid bool setting" << name << value;
    }
    int boolValue = parseBool(value.toStdString().c_str(), -1);
    if (boolValue != std::get<int>(setting->value())) {
        info("'{} has changed to '{}'", name.toStdString(), value.toStdString());
        xml_set(name.toStdString().c_str(), value.toStdString().c_str());
    }
}

#define TEXT(widget) widget->currentText().toLatin1().data()

void MainDialog::onApply()
{
    /* ~/.config/labwc/rc.xml */
    setInt(m_settings, "/labwc_config/theme/cornerRadius", ui->cornerRadius->value());
    setStr(m_settings, "/labwc_config/theme/name", TEXT(ui->openboxTheme));
    setBool(m_settings, "/labwc_config/theme/dropShadows", TEXT(ui->dropShadows));
    setStr(m_settings, "/labwc_config/theme/icon", TEXT(ui->iconTheme));
    setBool(m_settings, "/labwc_config/libinput/device/naturalScroll", TEXT(ui->naturalScroll));
    setStr(m_settings, "/labwc_config/placement/policy", TEXT(ui->placementPolicy));
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

bool hasOnlyCursorSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("cursors") && entries.length() == 1;
}

bool hasCursorSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("cursors");
}

QStringList MainDialog::findIconThemes(enum lab_icon_theme_type type)
{
    QStringList paths;

    // Setup paths including
    //   - $HOME/.icons
    //   - $XDG_DATA_HOME/icons
    //   - $XDG_DATA_DIRS/icons
    paths.push_back(QString(qgetenv("HOME") + "/.icons"));
    QStringList standardPaths =
            QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    for (const QString &path : std::as_const(standardPaths)) {
        paths.push_back(QString(path + "/icons"));
    }

    // Iterate over paths and use any icon-theme which has more than just a
    // "cursors" subdirectory (because that means it's for cursors only)
    QStringList themes;
    themes.push_front("Adwaita");
    for (const QString &path : std::as_const(paths)) {
        QDir dir(path);
        QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : std::as_const(entries)) {
            switch (type) {
            case LAB_ICON_THEME_TYPE_ICON:
                if (hasOnlyCursorSubdir(QString(path + "/" + entry))) {
                    continue;
                }
                themes.push_back(entry);
                break;
            case LAB_ICON_THEME_TYPE_CURSOR:
                if (hasCursorSubdir(QString(path + "/" + entry))) {
                    themes.push_back(entry);
                }
                break;
            default:
                break;
            }
        }
    }
    themes.removeDuplicates();
    themes.sort(Qt::CaseInsensitive);
    return themes;
}

bool hasOpenboxOrLabwcSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("openbox-3") || entries.contains("labwc");
}

QStringList MainDialog::findLabwcThemes(void)
{
    QStringList paths;

    paths.push_back(QString(qgetenv("HOME") + "/.themes"));
    QStringList standardPaths =
            QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    for (const QString &path : std::as_const(standardPaths)) {
        paths.push_back(QString(path + "/themes"));
    }

    QStringList themes;
    themes.push_front("Adwaita");
    for (const QString &path : std::as_const(paths)) {
        QDir dir(path);
        QStringList entries = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &entry : std::as_const(entries)) {
            if (hasOpenboxOrLabwcSubdir(QString(path + "/" + entry))) {
                themes.push_back(entry);
            }
        }
    }
    themes.removeDuplicates();
    themes.sort(Qt::CaseInsensitive);
    return themes;
}
