#include "about.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "./ui_about.h"

About::About(QWidget *parent) : QWidget(parent), ui(new Ui::pageAbout)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}

void About::loadInfo()
{
    QString version = QString::fromUtf8(qgetenv("LABWC_VER"));
    if (version.isEmpty())
        version = "Unknown";
    ui->versionValue->setText(version);

    // Features
    QString pid = QString::fromUtf8(qgetenv("LABWC_PID"));
    QString exePath = QString("/proc/%1/exe").arg(pid);

    QProcess proc;
    proc.start(exePath, { "-v" });
    proc.waitForFinished();

    // Fallback
    if (exePath.isEmpty() || !QFile::exists(exePath)) {
        proc.start("labwc", { "-v" });
        proc.waitForFinished();
    }

    QString out = proc.readAllStandardOutput().trimmed();

    if (out.isEmpty())
        out = proc.readAllStandardError().trimmed();

    ui->xwaylandValue->setText(out.contains("+xwayland") ? "✔" : "✘");
    ui->nlsValue->setText(out.contains("+nls") ? "✔" : "✘");
    ui->rsvgValue->setText(out.contains("+rsvg") ? "✔" : "✘");
    ui->libsfdoValue->setText(out.contains("+libsfdo") ? "✔" : "✘");

    QString labwcLink = QStringLiteral("<a href=\"https://labwc.github.io/\">labwc.github.io</a>");
    ui->labwcLinkValue->setText(labwcLink);
    ui->labwcLinkValue->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labwcLinkValue->setOpenExternalLinks(true);

    QString labwcTweaksVersion = QStringLiteral("0.1.0");
    ui->labwcTweaksVersionValue->setText(labwcTweaksVersion);

    QString labwcTweaksLink = QStringLiteral(
            "<a href=\"https://github.com/labwc/labwc-tweaks/\">github.com/labwc/labwc-tweaks</a>");
    ui->labwcTweaksLinkValue->setText(labwcTweaksLink);
    ui->labwcTweaksLinkValue->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->labwcTweaksLinkValue->setOpenExternalLinks(true);
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

    QString confdir = QString::fromUtf8(qgetenv("LABWC_CONFIG_DIR"));
    confdir.replace(QDir::homePath(), "~");

    if (confdir.isEmpty())
        confdir = "~/.config/labwc/";

    ui->configdirValue->setText(confdir);

    QString display = QString::fromUtf8(qgetenv("WAYLAND_DISPLAY"));
    if (display.isEmpty())
        display = "Unknown";

    ui->displayValue->setText(display);
}

void About::onApply()
{
    // No-op
}
