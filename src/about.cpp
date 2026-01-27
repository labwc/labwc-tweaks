#include "about.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include "./ui_about.h"

About::About(QWidget *parent) : QWidget(parent), ui(new Ui::pageAbout)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::loadLabwcVersion()
{
    QString version = QString::fromUtf8(qgetenv("LABWC_VER"));
    if (version.isEmpty())
        version = "Unknown";
    ui->versionValue->setText(version);

    // Features
    QString pid = QString::fromUtf8(qgetenv("LABWC_PID"));
    QString exePath = QString("/proc/%1/exe").arg(pid);

    QProcess proc;
    proc.start(exePath, {"-v"});
    proc.waitForFinished();

    // Fallback
    if (exePath.isEmpty() || !QFile::exists(exePath)) {
        proc.start("labwc", {"-v"});
        proc.waitForFinished();
    }

    QString out = proc.readAllStandardOutput().trimmed();

    if (out.isEmpty())
        out = proc.readAllStandardError().trimmed();

    ui->xwaylandValue->setText(out.contains("+xwayland") ? "✔" : "✘");
    ui->nlsValue->setText(out.contains("+nls") ? "✔" : "✘");
    ui->rsvgValue->setText(out.contains("+rsvg") ? "✔" : "✘");
    ui->libsfdoValue->setText(out.contains("+libsfdo") ? "✔" : "✘");
}

void About::getEnv()
{
    QString desktop = QString::fromUtf8(qgetenv("XDG_CURRENT_DESKTOP"));
    if (desktop.isEmpty())
        desktop = "Unknown";

    ui->desktopValue->setText(desktop);

    QString pid = QString::fromUtf8(qgetenv("LABWC_PID"));
    if (pid.isEmpty())
        pid = "Unknown";

    ui->pidValue->setText(pid);

    QString display = QString::fromUtf8(qgetenv("WAYLAND_DISPLAY"));
    if (display.isEmpty())
        display = "Unknown";

    ui->displayValue->setText(display);
}

void About::onApply()
{
    // No-op
}

