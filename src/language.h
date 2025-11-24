#ifndef LANGUAGE_H
#define LANGUAGE_H
#include <QWidget>
#include "layoutmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class pageLanguage;
}
QT_END_NAMESPACE

class Language : public QWidget
{
    Q_OBJECT

public:
    Language(QWidget *parent = nullptr);
    ~Language();

    void activate();
    void onApply();

private slots:
    void addSelectedLayout(void);
    void deleteSelectedLayout(void);

private:
    Ui::pageLanguage *ui;
    LayoutModel *m_model;
};
#endif // LANGUAGE_H
