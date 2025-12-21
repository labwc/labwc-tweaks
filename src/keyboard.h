#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <QWidget>
#include "layoutmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class pageKeyboard;
}
QT_END_NAMESPACE

class Keyboard : public QWidget
{
    Q_OBJECT

public:
    Keyboard(QWidget *parent = nullptr);
    ~Keyboard();

    void activate();
    void onApply();

private slots:
    void addSelectedLayout(void);
    void deleteSelectedLayout(void);

private:
    Ui::pageKeyboard *ui;
    LayoutModel *m_model;
};
#endif // KEYBOARD_H
