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

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(std::vector<std::shared_ptr<Setting>> &settings, QWidget *parent = nullptr);
    ~MainDialog();
    void activate();
    QStringList findIconThemes();

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
