#ifndef APPEARANCE_H
#define APPEARANCE_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageAppearance;
}
QT_END_NAMESPACE

class Appearance : public QWidget
{
    Q_OBJECT

public:
    Appearance(QWidget *parent = nullptr);
    ~Appearance();

    void activate();
    void onApply();

private:
    Ui::pageAppearance *ui;
};
#endif // APPEARANCE_H
