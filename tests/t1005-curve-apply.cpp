// SPDX-License-Identifier: GPL-2.0-only
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTemporaryDir>
#include <sys/wait.h>
#include "accel-curve-editor.h"
#include "environment.h"
#include "maindialog.h"
#include "mouse.h"
#include "settings.h"
#include "xml.h"
#include "tap.h"

static int nr_tests;
#define check(condition) do { ++nr_tests; ok1(condition); } while (0)

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QTemporaryDir directory;
    if (!directory.isValid()) {
        return EXIT_FAILURE;
    }
    // Keep Apply's reconfigure subprocess away from the user's compositor.
    QFile stub(directory.filePath("labwc"));
    if (!stub.open(QIODevice::WriteOnly)) {
        return EXIT_FAILURE;
    }
    stub.write("#!/bin/sh\nexit 0\n");
    stub.close();
    if (!stub.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner)) {
        return EXIT_FAILURE;
    }
    qputenv("PATH", QFile::encodeName(directory.path()));
    qunsetenv("LABWC_PID");
    qunsetenv("LABWC_TWEAKS_SHOW_TEMPLATE");
    environmentInit(directory.filePath("environment").toStdString());
    const QByteArray filename = QFile::encodeName(directory.filePath("rc.xml"));
    const char *curveNames[] = {"motionCurve", "scrollCurve", "fallbackCurve"};
    for (const char *initial : {"<accelProfile>adaptive</accelProfile>",
                               "<accelProfile>custom</accelProfile><scrollCurve />"}) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return EXIT_FAILURE;
        }
        file.write(QByteArray("<labwc_config><libinput><device>") + initial
                   + "</device></libinput></labwc_config>");
        file.close();
        check(xml_init(filename.constData()));
        std::vector<std::shared_ptr<Setting>> settings;
        settingsInit(&settings);
        {
            MainDialog dialog;
            auto mouse = dialog.findChild<Mouse *>();
            auto profile = mouse->findChild<QComboBox *>("accelProfile");
            auto apply = dialog.findChild<QDialogButtonBox *>()->button(QDialogButtonBox::Apply);
            check(apply->isEnabled() == (profile->currentData().toString() == "adaptive"));
            profile->setCurrentIndex(profile->findData("custom"));
            auto scroll = mouse->findChild<AccelCurveEditor *>("scrollCurve");
            scroll->findChild<QGroupBox *>("defined")->setChecked(false);
            check(apply->isEnabled());
            apply->click();
            wait(nullptr);
            for (const char *name : curveNames) {
                check(!xml_node_exists((std::string("/labwc_config/libinput/device/") + name).c_str()));
            }
            for (const char *name : curveNames) {
                auto curve = mouse->findChild<AccelCurveEditor *>(name);
                auto defined = curve->findChild<QGroupBox *>("defined");
                auto step = curve->findChild<QLineEdit *>("step");
                auto points = curve->findChild<QLineEdit *>("points");
                defined->setChecked(true);
                step->setText("0.10");
                points->setText("0 0.020 0.030 0.050 0.095 0.170 0.258 0.546 0.800 0.990");
                check(apply->isEnabled());
                points->setText("0 NaN");
                check(!apply->isEnabled());
                apply->click();
                check(!xml_node_exists((std::string("/labwc_config/libinput/device/") + name).c_str()));
                step->setText("0");
                points->setText("0 1");
                check(!apply->isEnabled());
                for (const char *other : {"flat", "adaptive"}) {
                    profile->setCurrentIndex(profile->findData(other));
                    check(apply->isEnabled() && !curve->isEnabled());
                }
                profile->setCurrentIndex(profile->findData("custom"));
                check(!apply->isEnabled() && curve->isEnabled());
                step->setText("0.10");
                check(apply->isEnabled());
                apply->click();
                wait(nullptr);
                check(xml_node_exists((std::string("/labwc_config/libinput/device/") + name).c_str()));
                defined->setChecked(false);
                apply->click();
                wait(nullptr);
                check(!xml_node_exists((std::string("/labwc_config/libinput/device/") + name).c_str()));
            }
        } // MainDialog releases the XML document.
    }
    plan(nr_tests);
    return exit_status();
}
