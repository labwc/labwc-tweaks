// SPDX-License-Identifier: GPL-2.0-only
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFile>
#include <QSlider>
#include <QTemporaryDir>
#include "mouse.h"
#include "settings.h"
#include "xml.h"
#include "tap.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QTemporaryDir directory;
    if (!directory.isValid()) {
        return EXIT_FAILURE;
    }
    const QByteArray filename = QFile::encodeName(directory.filePath("rc.xml"));
    const QByteArray curves =
        "      <motionCurve step=\"1\" points=\"0 1\" />\n"
        "      <scrollCurve step=\"0.10\" points=\"0 0.020 0.030 0.050 0.095 0.170 0.258 0.546 0.800 0.990\" />\n"
        "      <fallbackCurve step=\"1\" points=\"0 1\" />\n";
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return EXIT_FAILURE;
    }
    file.write("<labwc_config>\n  <libinput>\n    <device>\n"
               "      <accelProfile>custom</accelProfile>\n"
               "      <pointerSpeed>0.65</pointerSpeed>\n" + curves +
               "    </device>\n  </libinput>\n</labwc_config>\n");
    file.close();

    plan(13);
    ok1(xml_init(filename.constData()));
    std::vector<std::shared_ptr<Setting>> settings;
    settingsInit(&settings);
    {
        Mouse mouse;
        mouse.activate();
        auto profile = mouse.findChild<QComboBox *>("accelProfile");
        auto speed = mouse.findChild<QSlider *>("pointerSpeed");
        auto factor = mouse.findChild<QDoubleSpinBox *>("scrollFactor");
        ok1(profile->currentData().toString() == "custom");
        ok1(!speed->isEnabled());
        ok1(factor->isEnabled());
        for (const char *value : {"flat", "adaptive"}) {
            profile->setCurrentIndex(profile->findData(value));
            ok1(speed->isEnabled() && factor->isEnabled());
        }
        profile->setCurrentIndex(profile->findData("custom"));
        ok1(!speed->isEnabled() && factor->isEnabled());
        mouse.findChild<QCheckBox *>("leftHanded")->setChecked(true);
        mouse.onApply();
        xml_save();
        ok1(QString(xml_get("accelProfile.device.libinput.labwc_config")) == "custom");
        ok1(QString(xml_get("pointerSpeed.device.libinput.labwc_config")) == "0.65");
        ok1(xml_get_bool_text("leftHanded.device.libinput.labwc_config") == 1);
        ok1(file.open(QIODevice::ReadOnly) && file.readAll().contains(curves));
        file.close();
    }
    xml_finish();

    ok1(xml_init(filename.constData()));
    settings.clear();
    {
        Mouse reopened;
        reopened.activate();
        ok1(reopened.findChild<QComboBox *>("accelProfile")->currentData().toString() == "custom");
    }
    xml_finish();
    return exit_status();
}
