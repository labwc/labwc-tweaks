#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageTouchscreen;
}
QT_END_NAMESPACE

class Touchscreen : public QWidget
{
    Q_OBJECT

public:
    Touchscreen(QWidget *parent = nullptr);
    ~Touchscreen();

    void activate();
    void onApply();

private:
    Ui::pageTouchscreen *ui;
};
#endif // TOUCHSCREEN_H
