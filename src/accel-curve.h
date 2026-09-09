// SPDX-License-Identifier: GPL-2.0-only
#pragma once
#include <string>

enum class AccelCurveError { None, Step, Points };

// Match Labwc's positive step and 2..32 finite, non-negative output speeds.
// Decimal comma and scientific notation are accepted. Input is never modified.
AccelCurveError validateAccelCurve(const std::string &step, const std::string &points);
