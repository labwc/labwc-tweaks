#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include <QDialog>
#include "layoutmodel.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainDialog;
}
QT_END_NAMESPACE

enum lab_icon_theme_type {
    LAB_ICON_THEME_TYPE_NONE = 0,
    LAB_ICON_THEME_TYPE_ICON,
    LAB_ICON_THEME_TYPE_CURSOR,
};

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(std::vector<std::shared_ptr<Setting>> &settings, QWidget *parent = nullptr);
    ~MainDialog();
    void activate();
    QStringList findIconThemes(enum lab_icon_theme_type type);
    QStringList findLabwcThemes();

private slots:
    void addSelectedLayout(void);
    void deleteSelectedLayout(void);

private:
    LayoutModel *m_model;
    std::vector<std::shared_ptr<Setting>> &m_settings;

    void onApply();

    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
