#ifndef FIND_THEMES_H
#define FIND_THEMES_H

enum lab_icon_theme_type {
    LAB_ICON_THEME_TYPE_NONE = 0,
    LAB_ICON_THEME_TYPE_ICON,
    LAB_ICON_THEME_TYPE_CURSOR,
};

QStringList findIconThemes(enum lab_icon_theme_type type);
QStringList findLabwcThemes(void);

#endif // FIND_THEMES_H
