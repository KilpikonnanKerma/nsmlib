#ifndef VECTOR_HPP
#define VECTOR_HPP

#include "core.hpp"
#include <iostream>

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
        float length() const { return sqrt(x * x + y * y); }
        Vec2d normalized() const { float len = length(); return (len == 0) ? Vec2d() : (*this) / len; }
    };

    struct Vec4d {
        double x, y, z, w;

        Vec4d() : x(0), y(0), z(0), w(0) {}

        Vec4d(double x, double y, double z, double w)
            : x(x), y(y), z(z), w(w) {}

        Vec4d operator+(const Vec4d& other) const { return Vec4d(x + other.x, y + other.y, z + other.z, w + other.w); }
        Vec4d operator-(const Vec4d& other) const { return Vec4d(x - other.x, y - other.y, z - other.z, w - other.w); }
        Vec4d operator*(double scalar) const { return Vec4d(x * scalar, y * scalar, z * scalar, w * scalar); }
        Vec4d operator/(double scalar) const { return Vec4d(x / scalar, y / scalar, z / scalar, w / scalar); }
        Vec4d& operator+=(const Vec4d& v){ x+=v.x; y+=v.y; z+=v.z; w+=v.w; return *this; }
        Vec4d& operator-=(const Vec4d& v){ x-=v.x; y-=v.y; z-=v.z; w-=v.w; return *this; }
        Vec4d& operator*=(float s){ x*=s; y*=s; z*=s; w*=s; return *this; }
        Vec4d& operator/=(float s){ x/=s; y/=s; z/=s; w/=s; return *this; }

        double dot(const Vec4d& other) const { return x * other.x + y * other.y + z * other.z + w * other.w; }

        double length() const { return sqrt(x * x + y * y + z * z + w * w); }

        Vec4d normalized() const {
            double len = length();
            if (len == 0.0) return Vec4d(0,0,0,0);
            return (*this) / len;
        }
    };


    struct Vec3d {
        float x, y, z;
        Vec3d(float x, float y, float z) : x(x), y(y), z(z) {}
        Vec3d(float v = 0.0f) : x(v), y(v), z(v) {}

        Vec3d(const Vec4d& v) : x(v.x), y(v.y), z(v.z) {}

        Vec3d operator+(const Vec3d& v) const { return Vec3d(x + v.x, y + v.y, z + v.z); }
        Vec3d operator-(const Vec3d& v) const { return Vec3d(x - v.x, y - v.y, z - v.z); }
        Vec3d operator*(float s) const { return Vec3d(x * s, y * s, z * s); }
        Vec3d operator/(float s) const { return Vec3d(x / s, y / s, z / s); }
        Vec3d& operator+=(const Vec3d& v){ x+=v.x; y+=v.y; z+=v.z; return *this; }
        Vec3d& operator-=(const Vec3d& v){ x-=v.x; y-=v.y; z-=v.z; return *this; }
        Vec3d& operator*=(float s){ x*=s; y*=s; z*=s; return *this; }
        Vec3d& operator/=(float s){ x/=s; y/=s; z/=s; return *this; }


        float& operator[](int i) {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            throw std::out_of_range("Vec3d index out of range");
        }

        const float& operator[](int i) const {
            if (i == 0) return x;
            if (i == 1) return y;
            if (i == 2) return z;
            throw std::out_of_range("Vec3d index out of range");
        }

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

        Vec3d operator-() const { return Vec3d(-x, -y, -z); }
    };

    inline bool intersectRaySphere(const Vec3d& rayOrig, const Vec3d& rayDir,
                        const Vec3d& sphereCenter, double radiusSq, float& t)
    {
        Vec3d m = rayOrig - sphereCenter;
        double b = m.dot(rayDir);
        double c = m.dot(m) - radiusSq;

        if(c > 0.0 && b > 0.0) return false;

        double discr = b*b - c;
        if(discr < 0.0) return false;

        t = (float)(-b - sqrt(discr));
        if(t < 0.0f) t = 0.0f;
        return true;
    }
}

#endif