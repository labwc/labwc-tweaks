#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageTemplate;
}
QT_END_NAMESPACE

class Template : public QWidget
{
    Q_OBJECT

public:
    Template(QWidget *parent = nullptr);
    ~Template();

    void activate();
    void onApply();

private:
    Ui::pageTemplate *ui;
};
#endif // TEMPLATE_H
