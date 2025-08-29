#ifndef MAINDIALOG_H
#define MAINDIALOG_H
#include <QDialog>
#include "layoutmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainDialog;
}
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void activate();
    QStringList findIconThemes();

private slots:
    void addSelectedLayout(void);
    void deleteSelectedLayout(void);

private:
    LayoutModel *m_model;

    void onApply();

    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
