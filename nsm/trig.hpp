#ifndef TRIG_HPP
#define TRIG_HPP

#include "core.hpp"

namespace NMATH {
    inline float sin(float x) {
        x = wrapPi(x);
        // Core parabola approx: s ≈ a*x - b*x*|x|
        const float a = 1.2732395447351626862f;   // 4/PI
        const float b = 0.4052847345693510858f;   // 4/(PI*PI)
        float s = a * x - b * x * abs(x);
        const float p = 0.225f;
        s = p * (s * abs(s) - s) + s;
        return s;
    }

    inline float cos(float x) {
        return sin(x + HALF_PI);
    }

    inline float atan2(float y, float x) {
        const float ONEQTR_PI   = PI * 0.25f;
        const float THREEQTR_PI = PI * 0.75f;
        float abs_y = abs(y) + 1e-10f;
        float r, angle;
        if (x >= 0.0f) {
            r = (x - abs_y) / (x + abs_y);
            angle = ONEQTR_PI - ONEQTR_PI * r;
        } else {
            r = (x + abs_y) / (abs_y - x);
            angle = THREEQTR_PI - ONEQTR_PI * r;
        }
        return (y < 0.0f) ? -angle : angle;
    }
}

#endif