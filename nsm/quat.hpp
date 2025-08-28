#ifndef QUAT_HPP
#define QUAT_HPP

#include "core.hpp"
#include "trig.hpp"
#include "vector.hpp"

namespace NMATH {
    struct Quaternion {
        float x,y,z,w;
        Quaternion(float _x=0,float _y=0,float _z=0,float _w=1):x(_x),y(_y),z(_z),w(_w){}

        static Quaternion identity(){ return Quaternion(0,0,0,1); }

        Quaternion operator*(const Quaternion& q) const {
            return {
                w*q.x + x*q.w + y*q.z - z*q.y,
                w*q.y - x*q.z + y*q.w + z*q.x,
                w*q.z + x*q.y - y*q.x + z*q.w,
                w*q.w - x*q.x - y*q.y - z*q.z
            };
        }

        Quaternion& operator*=(const Quaternion& q){ *this = (*this)*q; return *this; }

        float length() const { return sqrt(x*x + y*y + z*z + w*w); }
        Quaternion normalized() const {
            float L = length();
            return (L<EPS) ? Quaternion() : Quaternion(x/L,y/L,z/L,w/L);
        }

        static Quaternion fromAxisAngle(const Vec3d& axis, float angleRad) {
            Vec3d a = axis.normalized();
            float half = angleRad * 0.5f;
            float s = sin(half);
            float c = cos(half);
            return Quaternion(a.x*s, a.y*s, a.z*s, c).normalized();
        }

        //normalized lerp + renorm
        static Quaternion nlerp(const Quaternion& a, const Quaternion& b, float t) {
            float dot = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
            float sign = (dot < 0.0f) ? -1.0f : 1.0f;
            Quaternion r( a.x + (b.x*sign - a.x)*t,
                    a.y + (b.y*sign - a.y)*t,
                    a.z + (b.z*sign - a.z)*t,
                    a.w + (b.w*sign - a.w)*t );
            return r.normalized();
        }
    };

    // rotate vec3d by quaternion
    inline Vec3d rotate(const Quaternion& q, const Vec3d& v) {
        // v' = q * (v,0) * q^-1
        Quaternion p(v.x, v.y, v.z, 0.0f);
        Quaternion qi(-q.x, -q.y, -q.z, q.w);
        Quaternion r = q * p * qi;
        return Vec3d(r.x, r.y, r.z);
    }
}

#endif