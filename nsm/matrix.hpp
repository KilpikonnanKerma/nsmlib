#ifndef MATRIX_HPP
#define MATRIX_HPP

#include "core.hpp"
#include "quat.hpp"

namespace NMATH {
    struct Mat4 {
        float m[4][4];

        static Mat4 identity() {
            Mat4 r{};
            r.m[0][0]=1; r.m[1][1]=1; r.m[2][2]=1; r.m[3][3]=1;
            return r;
        }

        Mat4 operator*(const Mat4& o) const {
            Mat4 r{};
            for (int i=0;i<4;i++)
                for (int j=0;j<4;j++) {
                    float s=0;
                    for (int k=0;k<4;k++) s += m[i][k]*o.m[k][j];
                    r.m[i][j]=s;
                }
            return r;
        }

        // Transform a point (assumes w=1)
        Vec3d transformPoint(const Vec3d& v) const {
            float x_ = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3];
            float y_ = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3];
            float z_ = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3];
            float w_ = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3];
            if (abs(w_) > EPS) { float invW = 1.0f / w_; x_*=invW; y_*=invW; z_*=invW; }
            return {x_,y_,z_};
        }

        // Transform a direction (assumes w=0)
        Vec3d transformDir(const Vec3d& v) const {
            return {
                m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
                m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
                m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z
            };
        }

        static Mat4 fromQuat(const Quaternion& qn) {
            Quaternion q = qn.normalized();
            float xx=q.x*q.x, yy=q.y*q.y, zz=q.z*q.z;
            float xy=q.x*q.y, xz=q.x*q.z, yz=q.y*q.z;
            float wx=q.w*q.x, wy=q.w*q.y, wz=q.w*q.z;

            Mat4 r = identity();
            r.m[0][0] = 1.0f - 2.0f*(yy+zz);
            r.m[0][1] =        2.0f*(xy - wz);
            r.m[0][2] =        2.0f*(xz + wy);

            r.m[1][0] =        2.0f*(xy + wz);
            r.m[1][1] = 1.0f - 2.0f*(xx+zz);
            r.m[1][2] =        2.0f*(yz - wx);

            r.m[2][0] =        2.0f*(xz - wy);
            r.m[2][1] =        2.0f*(yz + wx);
            r.m[2][2] = 1.0f - 2.0f*(xx+yy);
            return r;
        }
    };

    // Transform builders
    inline Mat4 translate(const Vec3d& t) {
        Mat4 r = Mat4::identity();
        r.m[0][3] = t.x; r.m[1][3] = t.y; r.m[2][3] = t.z;
        return r;
    }

    inline Mat4 scale(const Vec3d& s) {
        Mat4 r{};
        r.m[0][0]=s.x; r.m[1][1]=s.y; r.m[2][2]=s.z; r.m[3][3]=1.0f;
        return r;
    }

    inline Mat4 rotateX(float a) {
        float c = cos(a), s = sin(a);
        Mat4 r = Mat4::identity();
        r.m[1][1]=c; r.m[1][2]=-s; r.m[2][1]=s; r.m[2][2]=c;
        return r;
    }
    inline Mat4 rotateY(float a) {
        float c = cos(a), s = sin(a);
        Mat4 r = Mat4::identity();
        r.m[0][0]=c; r.m[0][2]=s; r.m[2][0]=-s; r.m[2][2]=c;
        return r;
    }
    inline Mat4 rotateZ(float a) {
        float c = cos(a), s = sin(a);
        Mat4 r = Mat4::identity();
        r.m[0][0]=c; r.m[0][1]=-s; r.m[1][0]=s; r.m[1][1]=c;
        return r;
    }

    inline Mat4 rotateAxisAngle(const Vec3d& axis, float angleRad) {
        return Mat4::fromQuat(Quaternion::fromAxisAngle(axis, angleRad));
    }

    // Camera & projection
    inline Mat4 lookAt(const Vec3d& eye, const Vec3d& center, const Vec3d& up) {
        Vec3d f = (center - eye).normalized();
        Vec3d r = f.cross(up).normalized();
        Vec3d u = r.cross(f);

        Mat4 m = Mat4::identity();
        m.m[0][0]= r.x; m.m[0][1]= r.y; m.m[0][2]= r.z; m.m[0][3]= -r.dot(eye);
        m.m[1][0]= u.x; m.m[1][1]= u.y; m.m[1][2]= u.z; m.m[1][3]= -u.dot(eye);
        m.m[2][0]=-f.x; m.m[2][1]=-f.y; m.m[2][2]=-f.z; m.m[2][3]=  f.dot(eye);
        // m.m[3][3]=1 already from identity
        return m;
    }

    inline Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) {
        // Right-handed, depth in [0,1] after perspective divide depends on your API.
        float f = 1.0f / tanHalf(fovyRad);
        Mat4 m{};
        m.m[0][0] = f / aspect;
        m.m[1][1] = f;
        m.m[2][2] = (zFar + zNear) / (zNear - zFar);
        m.m[2][3] = (2.0f * zFar * zNear) / (zNear - zFar);
        m.m[3][2] = -1.0f;
        return m;
    }

    inline Mat4 orthographic(float l, float r, float b, float t, float n, float fz) {
        Mat4 m{};
        m.m[0][0] = 2.0f / (r - l);
        m.m[1][1] = 2.0f / (t - b);
        m.m[2][2] = -2.0f / (fz - n);
        m.m[0][3] = -(r + l) / (r - l);
        m.m[1][3] = -(t + b) / (t - b);
        m.m[2][3] = -(fz + n) / (fz - n);
        m.m[3][3] = 1.0f;
        return m;
    }
}

#endif