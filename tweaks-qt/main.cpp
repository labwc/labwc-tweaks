#include "maindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("labwc-tweaks"));

    MainDialog w;
    w.show();

    return app.exec();
}
