#include <QDir>
#include <QString>
#include <QStandardPaths>
#include "find-themes.h"
#include "log.h"

static bool hasOnlyCursorSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("cursors") && entries.length() == 1;
}

static bool hasCursorSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("cursors");
}

QStringList findIconThemes(enum lab_icon_theme_type type)
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
    themes.push_front("");
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

static bool hasOpenboxOrLabwcSubdir(QString path)
{
    QStringList entries = QDir(path).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    return entries.contains("openbox-3") || entries.contains("labwc");
}

QStringList findLabwcThemes(void)
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
