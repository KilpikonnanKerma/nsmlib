#ifndef TRIG_HPP
#define TRIG_HPP

#include "core.hpp"

namespace NMATH {
    inline float sin(float x) {
        x = wrapPi(x);
        // Core parabola approx: s ≈ a*x - b*x*|x|
        const float a = 1.2732395447351626862f;   // 4/PI
        const float b = 0.4052847345693510858f;   // 4/(PI*PI)
        float s = a * x - b * x * absf(x);
        const float p = 0.225f;
        s = p * (s * absf(s) - s) + s;
        return s;
    }

    inline float cos(float x) {
        return sin(x + HALF_PI);
    }

    inline float tan(float x) {
        float s = sin(x);
        float c = cos(x);
        if (absf(c) < 1e-6f) c = (c>=0 ? 1e-6f : -1e-6f);
        return s / c;
    }

    inline float atan2(float y, float x) {
        const float ONEQTR_PI   = PI * 0.25f;
        const float THREEQTR_PI = PI * 0.75f;
        float abs_y = absf(y) + 1e-10f;
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

    inline float tanHalf(float fovyRad){
        float s = sin(fovyRad * 0.5f);
        float c = cos(fovyRad * 0.5f);
        // avoid divide-by-zero with crude guard
        if (absf(c) < 1e-6f) c = (c>=0 ? 1e-6f : -1e-6f);
        return s / c;
    }
}

#endif