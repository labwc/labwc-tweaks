#ifndef ABOUT_H
#define ABOUT_H
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class pageAbout;
}
QT_END_NAMESPACE

class About : public QWidget
{
    Q_OBJECT

public:
    About(QWidget *parent = nullptr);
    ~About();

    void onApply();
    void loadLabwcVersion();
    void getEnv();

private:
    Ui::pageAbout *ui;
};
#endif // ABOUT_H
