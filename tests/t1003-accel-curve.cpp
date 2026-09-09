// SPDX-License-Identifier: GPL-2.0-only
#include "accel-curve.h"
#include "tap.h"

int main()
{
    const struct {
        const char *step;
        const char *points;
        AccelCurveError error;
    } cases[] = {
        {"1", "0 1", AccelCurveError::None},
        {"0.10", "0 0.020 0.030 0.050 0.095 0.170 0.258 0.546 0.800 0.990", AccelCurveError::None},
        {" 0,10 ", " 0\t0,5\r\n1 ", AccelCurveError::None},
        {"1e-100", "0 1e100", AccelCurveError::None},
        {"0x1p0", "0 0x1p1", AccelCurveError::None},
        {"1", "1 0", AccelCurveError::None},
        {"1", "-0 +1", AccelCurveError::None},
        {"1", "0", AccelCurveError::Points},
        {"1", "", AccelCurveError::Points},
        {"1", " \t ", AccelCurveError::Points},
        {"1", "0 -1", AccelCurveError::Points},
        {"1", "0 word", AccelCurveError::Points},
        {"1", "0 1oops", AccelCurveError::Points},
        {"1", "0 0.1.2", AccelCurveError::Points},
        {"1", "0 0,1.2", AccelCurveError::Points},
        {"1", "0 NaN", AccelCurveError::Points},
        {"1", "0 Infinity", AccelCurveError::Points},
        {"1", "0 -inf", AccelCurveError::Points},
        {"1", "0 1e999", AccelCurveError::Points},
        {"1", "0 1e-999", AccelCurveError::Points},
        {"0", "0 1", AccelCurveError::Step},
        {"-1", "0 1", AccelCurveError::Step},
        {"nan", "0 1", AccelCurveError::Step},
        {"inf", "0 1", AccelCurveError::Step},
        {"-Infinity", "0 1", AccelCurveError::Step},
        {"1e999", "0 1", AccelCurveError::Step},
        {"1e-999", "0 1", AccelCurveError::Step},
        {"", "0 1", AccelCurveError::Step},
        {" ", "0 1", AccelCurveError::Step},
        {"1garbage", "0 1", AccelCurveError::Step},
        {"1.0.0", "0 1", AccelCurveError::Step},
    };
    plan(sizeof(cases) / sizeof(cases[0]) + 4);
    for (const auto &test : cases) {
        ok(validateAccelCurve(test.step, test.points) == test.error,
           "step='%s', points='%s'", test.step, test.points);
    }
    std::string points;
    for (int i = 0; i < 32; ++i) {
        points += std::to_string(i) + " ";
    }
    ok1(validateAccelCurve("1", points) == AccelCurveError::None);
    ok1(validateAccelCurve("1", points + "32") == AccelCurveError::Points);
    ok1(validateAccelCurve(std::string("1\0junk", 6), "0 1") == AccelCurveError::Step);
    ok1(validateAccelCurve("1", std::string("0 1\0junk", 8)) == AccelCurveError::Points);
    return exit_status();
}
