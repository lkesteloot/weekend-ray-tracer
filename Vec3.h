#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

/**
 * Vector class with all the usual operators.
 */
class Vec3 {
public:
    float mE[3];

    Vec3() {
        // Nothing.
    }
    Vec3(float e0, float e1, float e2) {
        mE[0] = e0;
        mE[1] = e1;
        mE[2] = e2;
    }
    inline float x() const { return mE[0]; }
    inline float y() const { return mE[1]; }
    inline float z() const { return mE[2]; }
    inline float r() const { return mE[0]; }
    inline float g() const { return mE[1]; }
    inline float b() const { return mE[2]; }

    inline const Vec3 &operator+() const { return *this; }
    inline Vec3 operator-() const { return Vec3(-mE[0], -mE[1], -mE[2]); }
    inline float operator[](int i) const { return mE[i]; }
    inline float &operator[](int i) { return mE[i]; }

    inline Vec3 operator+(const Vec3 &o) const {
        return Vec3(mE[0] + o.mE[0], mE[1] + o.mE[1], mE[2] + o.mE[2]);
    }
    inline Vec3 operator-(const Vec3 &o) const {
        return Vec3(mE[0] - o.mE[0], mE[1] - o.mE[1], mE[2] - o.mE[2]);
    }
    inline Vec3 operator*(const Vec3 &o) const {
        return Vec3(mE[0]*o.mE[0], mE[1]*o.mE[1], mE[2]*o.mE[2]);
    }
    inline Vec3 operator/(const Vec3 &o) const {
        return Vec3(mE[0]/o.mE[0], mE[1]/o.mE[1], mE[2]/o.mE[2]);
    }
    inline Vec3 operator*(float t) const {
        return Vec3(mE[0]*t, mE[1]*t, mE[2]*t);
    }
    inline Vec3 operator/(float t) const {
        return Vec3(mE[0]/t, mE[1]/t, mE[2]/t);
    }
    inline Vec3 &operator+=(const Vec3 &o) {
        mE[0] += o.mE[0];
        mE[1] += o.mE[1];
        mE[2] += o.mE[2];
        return *this;
    }
    inline Vec3 &operator-=(const Vec3 &o) {
        mE[0] -= o.mE[0];
        mE[1] -= o.mE[1];
        mE[2] -= o.mE[2];
        return *this;
    }
    inline Vec3 &operator*=(const Vec3 &o) {
        mE[0] *= o.mE[0];
        mE[1] *= o.mE[1];
        mE[2] *= o.mE[2];
        return *this;
    }
    inline Vec3 &operator/=(const Vec3 &o) {
        mE[0] /= o.mE[0];
        mE[1] /= o.mE[1];
        mE[2] /= o.mE[2];
        return *this;
    }
    inline Vec3 &operator*=(float t) {
        mE[0] *= t;
        mE[1] *= t;
        mE[2] *= t;
        return *this;
    }
    inline Vec3 &operator/=(float t) {
        float recip = 1/t;
        mE[0] *= recip;
        mE[1] *= recip;
        mE[2] *= recip;
        return *this;
    }
    inline Vec3 unit() const {
        return *this/length();
    }
    inline float dot(const Vec3 &o) const {
        return mE[0]*o.mE[0] + mE[1]*o.mE[1] + mE[2]*o.mE[2];
    }

    inline Vec3 cross(const Vec3 &o) {
        return Vec3(
                mE[1]*o.mE[2] - mE[2]*o.mE[1],
                mE[2]*o.mE[0] - mE[0]*o.mE[2],
                mE[0]*o.mE[1] - mE[1]*o.mE[0]);
    }

    inline float length() const {
        return sqrt(squared_length());
    }
    inline float squared_length() const {
        return dot(*this);
    }
    inline void make_unit_vector() {
        *this /= length();
    }
};

inline Vec3 operator*(float t, const Vec3 &v) {
    return v*t;
}
inline std::istream &operator>>(std::istream &is, Vec3 &v) {
    is >> v.mE[0] >> v.mE[1] >> v.mE[2];
    return is;
}
inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
    os << v.mE[0] << " " << v.mE[1] << " " << v.mE[2];
    return os;
}

Vec3 random_in_unit_sphere();

static Vec3 VEC3_BLACK = Vec3(0, 0, 0);
static Vec3 VEC3_ORIGIN = Vec3(0, 0, 0);
static Vec3 VEC3_ONES = Vec3(1, 1, 1);

#endif // VEC3_H
