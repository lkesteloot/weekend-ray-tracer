#include <math.h>
#include <stdlib.h>
#include <iostream>

/**
 * Vector class with all the usual operators.
 */
class Vec3 {
public:
    float e[3];

    Vec3() {
        // Nothing.
    }
    Vec3(float e0, float e1, float e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }
    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const Vec3 &operator+() const { return *this; }
    inline Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float &operator[](int i) { return e[i]; }

    inline Vec3 operator+(const Vec3 &o) const {
        return Vec3(e[0] + o.e[0], e[1] + o.e[1], e[2] + o.e[2]);
    }
    inline Vec3 operator-(const Vec3 &o) const {
        return Vec3(e[0] - o.e[0], e[1] - o.e[1], e[2] - o.e[2]);
    }
    inline Vec3 operator*(const Vec3 &o) const {
        return Vec3(e[0]*o.e[0], e[1]*o.e[1], e[2]*o.e[2]);
    }
    inline Vec3 operator/(const Vec3 &o) const {
        return Vec3(e[0]/o.e[0], e[1]/o.e[1], e[2]/o.e[2]);
    }
    inline Vec3 operator*(float t) const {
        return Vec3(e[0]*t, e[1]*t, e[2]*t);
    }
    inline Vec3 operator/(float t) const {
        return Vec3(e[0]/t, e[1]/t, e[2]/t);
    }
    inline Vec3 &operator+=(const Vec3 &o) {
        e[0] += o.e[0];
        e[1] += o.e[1];
        e[2] += o.e[2];
        return *this;
    }
    inline Vec3 &operator-=(const Vec3 &o) {
        e[0] -= o.e[0];
        e[1] -= o.e[1];
        e[2] -= o.e[2];
        return *this;
    }
    inline Vec3 &operator*=(const Vec3 &o) {
        e[0] *= o.e[0];
        e[1] *= o.e[1];
        e[2] *= o.e[2];
        return *this;
    }
    inline Vec3 &operator/=(const Vec3 &o) {
        e[0] /= o.e[0];
        e[1] /= o.e[1];
        e[2] /= o.e[2];
        return *this;
    }
    inline Vec3 &operator*=(float t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }
    inline Vec3 &operator/=(float t) {
        e[0] /= t;
        e[1] /= t;
        e[2] /= t;
        return *this;
    }
    inline Vec3 unit() const {
        return *this / length();
    }
    inline float dot(const Vec3 &o) const {
        return e[0]*o.e[0] + e[1]*o.e[1] + e[2]*o.e[2];
    }

    inline Vec3 cross(const Vec3 &o) {
        return Vec3(
                e[1]*o.e[2] - e[2]*o.e[1],
                e[2]*o.e[0] - e[0]*o.e[2],
                e[0]*o.e[1] - e[1]*o.e[0]);
    }

    inline float length() const {
        return sqrt(squared_length());
    }
    inline float squared_length() const {
        return dot(*this);
    }
    inline void make_unit_vector() {
        *this *= 1/length();
    }
};

inline Vec3 operator*(float t, const Vec3 &v) {
    return v*t;
}
inline std::istream &operator>>(std::istream &is, Vec3 &v) {
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}
inline std::ostream &operator<<(std::ostream &os, const Vec3 &v) {
    os << v.e[0] << " " << v.e[1] << " " << v.e[2];
    return os;
}
