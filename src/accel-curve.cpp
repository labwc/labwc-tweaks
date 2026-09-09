// SPDX-License-Identifier: GPL-2.0-only
#include "accel-curve.h"
#include <cerrno>
#include <cmath>
#include <glib.h>

static bool parseNumber(std::string text, double &value)
{
    // Labwc accepts either decimal separator, independently of the user's locale.
    bool separator = false;
    for (char &c : text) {
        if (c == '.' || c == ',') {
            if (separator) {
                return false;
            }
            separator = true;
            c = '.';
        }
    }
    char *end = nullptr;
    errno = 0;
    value = g_ascii_strtod(text.c_str(), &end);
    return end != text.c_str() && !*end && !errno && std::isfinite(value);
}

AccelCurveError validateAccelCurve(const std::string &step, const std::string &points)
{
    if (step.find('\0') != std::string::npos) {
        return AccelCurveError::Step;
    }
    if (points.find('\0') != std::string::npos) {
        return AccelCurveError::Points;
    }
    gchar *trimmed = g_strdup(step.c_str());
    double value = 0;
    bool valid = parseNumber(g_strstrip(trimmed), value) && value > 0;
    g_free(trimmed);
    if (!valid) {
        return AccelCurveError::Step;
    }

    gchar **tokens = g_strsplit_set(points.c_str(), " \t\r\n", -1);
    unsigned int nr_points = 0;
    for (gchar **token = tokens; *token; ++token) {
        if (!**token) {
            continue;
        }
        if (++nr_points > 32 || !parseNumber(*token, value) || value < 0) {
            valid = false;
            break;
        }
    }
    g_strfreev(tokens);
    return valid && nr_points >= 2 ? AccelCurveError::None : AccelCurveError::Points;
}
