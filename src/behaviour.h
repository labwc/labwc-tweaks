#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageBehaviour;
}
QT_END_NAMESPACE

class Behaviour : public QWidget
{
    Q_OBJECT

public:
    Behaviour(QWidget *parent = nullptr);
    ~Behaviour();

    void activate();
    void onApply();

private:
    Ui::pageBehaviour *ui;
};
#endif // BEHAVIOUR_H
