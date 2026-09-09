#ifndef MOUSE_H
#define MOUSE_H
#include <QWidget>
#include <array>

class AccelCurveEditor;

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
    bool isValid() const;

signals:
    void validityChanged(bool valid);

private:
    Ui::pageMouse *ui;
    std::array<AccelCurveEditor *, 3> m_curves;
};
#endif // MOUSE_H
