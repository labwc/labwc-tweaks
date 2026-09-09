// SPDX-License-Identifier: GPL-2.0-only
#include <QApplication>
#include <QFile>
#include <QGroupBox>
#include <QLineEdit>
#include <QTemporaryDir>
#include "accel-curve-editor.h"
#include "settings.h"
#include "xml.h"
#include "tap.h"

static int nr_tests;
#define check(condition) do { ++nr_tests; ok1(condition); } while (0)

static QByteArray readFile(const QByteArray &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return file.readAll();
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QTemporaryDir directory;
    if (!directory.isValid()) {
        return EXIT_FAILURE;
    }
    const QByteArray filename = QFile::encodeName(directory.filePath("rc.xml"));
    const QByteArray points = "0 0.020 0.030 0.050 0.095 0.170 0.258 0.546 0.800 0.990";
    const QByteArray changed = "0 0.021 0.030 0.050 0.095 0.170 0.258 0.546 0.800 0.990";
    const QByteArray fixtures[] = {
        "<scrollCurve step=\"0.10\" points=\"" + points + "\" />",
        "<scrollCurve><step>0.10</step><points>" + points + "</points></scrollCurve>",
        "<scrollCurve step=\"0.10\"><points>" + points + "</points></scrollCurve>",
        "<scrollCurve points=\"" + points + "\"><step>0.10</step></scrollCurve>",
    };
    for (const auto &fixture : fixtures) {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            return EXIT_FAILURE;
        }
        file.write("<labwc_config><libinput><device><accelProfile>custom</accelProfile>"
                   + fixture + "</device></libinput></labwc_config>");
        file.close();
        check(xml_init(filename.constData()));
        std::vector<std::shared_ptr<Setting>> settings;
        settingsInit(&settings);
        {
            AccelCurveEditor editor("scrollCurve", "Scroll");
            editor.activate();
            auto defined = editor.findChild<QGroupBox *>("defined");
            auto step = editor.findChild<QLineEdit *>("step");
            auto values = editor.findChild<QLineEdit *>("points");
            check(defined->isChecked());
            check(step->text() == "0.10");
            check(values->text() == points);
            check(editor.isValid());
            check(getStr("/labwc_config/libinput/device/scrollCurve/step") == "0.10");
            editor.onApply();
            xml_save();
            QByteArray original = readFile(filename);
            check(original.contains("0.10") && original.contains(points));

            values->setText(changed);
            editor.onApply();
            xml_save();
            QByteArray written = readFile(filename);
            check(written.contains("<step>0.10</step>"));
            check(written.contains("<points>" + changed + "</points>"));
            check(!written.contains(" step=") && !written.contains(" points="));
            check(written.count("<scrollCurve>") == 1 && written.count("<step>") == 1
                  && written.count("<points>") == 1);
            check(!xml_node_exists("/labwc_config/libinput/device/motionCurve"));
            check(!xml_node_exists("/labwc_config/libinput/device/fallbackCurve"));
            editor.onApply();
            xml_save();
            check(readFile(filename) == written);

            defined->setChecked(false);
            editor.onApply();
            check(!xml_node_exists("/labwc_config/libinput/device/scrollCurve"));
            editor.onApply();
            check(!xml_node_exists("/labwc_config/libinput/device/scrollCurve"));
            defined->setChecked(true);
            editor.onApply();
            check(xml_node_exists("/labwc_config/libinput/device/scrollCurve"));
            check(getStr("/labwc_config/libinput/device/scrollCurve/step") == "0.10");
            check(getStr("/labwc_config/libinput/device/scrollCurve/points") == changed);
            xml_save();
        }
        xml_finish();
        check(xml_init(filename.constData()));
        settings.clear();
        {
            AccelCurveEditor reopened("scrollCurve", "Scroll");
            reopened.activate();
            check(reopened.findChild<QLineEdit *>("step")->text() == "0.10");
            check(reopened.findChild<QLineEdit *>("points")->text() == changed);
        }
        xml_remove_node("/labwc_config/libinput/device/SCROLLCURVE");
        check(!xml_node_exists("/labwc_config/libinput/device/scrollCurve"));
        xml_remove_node("/labwc_config/libinput/device/scrollCurve");
        check(xml_node_exists("/labwc_config/libinput/device/accelProfile"));
        xml_remove_node("/labwc_config");
        check(xml_node_exists("/labwc_config"));
        xpath_add_node("/labwc_config/libinput/device/scrollCurve");
        settings.clear();
        {
            AccelCurveEditor empty("scrollCurve", "Scroll");
            empty.activate();
            check(empty.findChild<QGroupBox *>("defined")->isChecked());
            check(!empty.isValid());
        }
        xml_finish();
    }
    plan(nr_tests);
    return exit_status();
}
