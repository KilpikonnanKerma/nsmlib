#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "core.hpp"

namespace NMATH {
    struct Vec2d {
        float x, y;
        Vec2d(float x = 0, float y = 0) : x(x), y(y) {}

        Vec2d operator+(const Vec2d& v) const { return Vec2d(x + v.x, y + v.y); }
        Vec2d operator-(const Vec2d& v) const { return Vec2d(x - v.x, y - v.y); }
        Vec2d operator*(float s) const { return Vec2d(x * s, y * s); }
        Vec2d operator/(float s) const { return Vec2d(x / s, y / s); }
        Vec2d& operator+=(const Vec2d& v){ x+=v.x; y+=v.y; return *this; }
        Vec2d& operator-=(const Vec2d& v){ x-=v.x; y-=v.y; return *this; }
        Vec2d& operator*=(float s){ x*=s; y*=s; return *this; }
        Vec2d& operator/=(float s){ x/=s; y/=s; return *this; }

        float dot(const Vec2d& v) const { return x * v.x + y * v.y; }
        float length() const { return NMATH::sqrt(x * x + y * y); }
        Vec2d normalized() const { float len = length(); return (len == 0) ? Vec2d() : (*this) / len; }
    };

    struct Vec3d {
        float x, y, z;
        Vec3d(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        Vec3d operator+(const Vec3d& v) const { return Vec3d(x + v.x, y + v.y, z + v.z); }
        Vec3d operator-(const Vec3d& v) const { return Vec3d(x - v.x, y - v.y, z - v.z); }
        Vec3d operator*(float s) const { return Vec3d(x * s, y * s, z * s); }
        Vec3d operator/(float s) const { return Vec3d(x / s, y / s, z / s); }
        Vec3d& operator+=(const Vec3d& v){ x+=v.x; y+=v.y; z+=v.z; return *this; }
        Vec3d& operator-=(const Vec3d& v){ x-=v.x; y-=v.y; z-=v.z; return *this; }
        Vec3d& operator*=(float s){ x*=s; y*=s; z*=s; return *this; }
        Vec3d& operator/=(float s){ x/=s; y/=s; z/=s; return *this; }

        float dot(const Vec3d& v) const { return x * v.x + y * v.y + z * v.z; }
        Vec3d cross(const Vec3d& v) const {
            return Vec3d(
                y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x
            );
        }
        float length() const { return sqrt(x * x + y * y + z * z); }
        Vec3d normalized() const { float len = length(); return (len == 0) ? Vec3d() : (*this) / len; }
    };
}

#endif