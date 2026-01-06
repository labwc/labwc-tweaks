#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include <QDialog>
#include <QDialogButtonBox>
#include "settings.h"

class Appearance;
class Behaviour;
class Mouse;
class Keyboard;
class Touchscreen;
class Template;

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void activate();

private:
    void onApply();

    QDialogButtonBox *m_buttonBox;
    Appearance *m_pageAppearance;
    Behaviour *m_pageBehaviour;
    Mouse *m_pageMouse;
    Keyboard *m_pageKeyboard;
    Touchscreen *m_pageTouchscreen;
    Template *m_pageTemplate;
};
#endif // MAINDIALOG_H
