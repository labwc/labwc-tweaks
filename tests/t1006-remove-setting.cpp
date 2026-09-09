// SPDX-License-Identifier: GPL-2.0-only
#include <fstream>
#include <string>
#include <unistd.h>
#include "xml.h"
#include "tap.h"

int main()
{
    char filename[] = "/tmp/t1006-XXXXXX";
    int fd = mkstemp(filename);
    if (fd < 0) {
        return EXIT_FAILURE;
    }
    close(fd);
    {
        std::ofstream file(filename);
        file << "<labwc_config><libinput><device>"
                "<scrollCurve step='1' points='0 1'/>"
                "<scrollCurve><step>2</step><points>0 2</points></scrollCurve>"
                "<motionCurve step='1' points='0 1'/>"
                "<fallbackCurve/>"
                "</device></libinput></labwc_config>";
    }
    plan(10);
    ok1(xml_init(filename));
    ok1(xml_node_exists("/labwc_config/libinput/device/fallbackCurve"));
    xml_remove_node("/labwc_config/libinput/device/SCROLLCURVE");
    ok1(!xml_node_exists("/labwc_config/libinput/device/scrollCurve"));
    ok1(std::string(xml_get("points.motionCurve.device.libinput.labwc_config")) == "0 1");
    xml_remove_node("/labwc_config/libinput/device/scrollCurve");
    ok1(xml_node_exists("/labwc_config/libinput/device/motionCurve"));
    xml_remove_node("/labwc_config/libinput/device/fallbackCurve");
    ok1(!xml_node_exists("/labwc_config/libinput/device/fallbackCurve"));
    xml_remove_node("/labwc_config/libinput/device/motionCurve/step");
    ok1(!xml_node_exists("/labwc_config/libinput/device/motionCurve/step"));
    xml_remove_node("/labwc_config");
    ok1(xml_node_exists("/labwc_config"));
    xml_save();
    xml_finish();
    ok1(xml_init(filename));
    ok1(!xml_node_exists("/labwc_config/libinput/device/scrollCurve")
        && xml_node_exists("/labwc_config/libinput/device/motionCurve"));
    xml_finish();
    unlink(filename);
    return exit_status();
}
