#ifndef MOUSE_H
#define MOUSE_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageMouse;
}
QT_END_NAMESPACE

class Mouse : public QWidget
{
    Q_OBJECT

public:
    Mouse(QWidget *parent = nullptr);
    ~Mouse();

    void activate();
    void onApply();

private:
    Ui::pageMouse *ui;
};
#endif // MOUSE_H
