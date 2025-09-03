#include <cstdint>
#include <cstring>

#ifndef CORE_HPP
#define CORE_HPP

namespace NMATH {

    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI  = 6.28318530717958647692f;
    constexpr float HALF_PI = 1.57079632679489661923f;
    constexpr float EPS     = 1e-6f;

    inline float absf(float v) { return (v < 0) ? -v : v; }
    inline int absi(int v) { return (v < 0) ? -v : v; }

    inline float min(float a, float b) { return (a < b) ? a : b; }
    inline float max(float a, float b) { return (a > b) ? a : b; }

    inline float clamp(float v, float lo, float hi) {
        return (v < lo) ? lo : (v > hi ? hi : v);
    }

    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    inline uint32_t floatToBits(float f) { uint32_t u; std::memcpy(&u, &f, sizeof(float)); return u; }
    inline float    bitsToFloat(uint32_t u){ float f; std::memcpy(&f, &u, sizeof(uint32_t)); return f; }

    inline float invSqrt(float x) {
        long i;
        float x2, y;
        const float threehalfs = 1.5f;

        x2 = x * 0.5F;
        y  = x;
        i  = *(long*)&y;
        i  = 0x5f3759df - (i >> 1);
        y  = *(float*)&i;
        y  = y * (threehalfs - (x2 * y * y));
        return y;
    }

    inline float sqrt(float x) { return x * invSqrt(x); }

    inline float radians(float deg) { return deg * (PI / 180.0f); }
    inline float degrees(float rad) { return rad * (180.0f / PI); }

    inline float wrapPi(float a) {
        while (a >  PI) a -= TWO_PI;
        while (a < -PI) a += TWO_PI;
        return a;
    }

}

#endif