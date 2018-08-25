
#include "Vec3.h"

static Vec3 VEC_XY_ONES = Vec3(1, 1, 0);

Vec3 random_in_unit_sphere() {
    Vec3 p;

    do {
        p = 2.0*Vec3(drand48(), drand48(), drand48()) - VEC3_ONES;
    } while (p.squared_length() > 1.0);

    return p;
}

Vec3 random_in_unit_disc() {
    Vec3 p;

    do {
        p = 2.0*Vec3(drand48(), drand48(), 0) - VEC_XY_ONES;
    } while (p.squared_length() > 1.0);

    return p;
}

bool refract(const Vec3 &v, const Vec3 &n, float ni_over_nt, Vec3 &refracted) {
    float dt = v.dot(n);
    float discriminant = 1 - ni_over_nt*ni_over_nt*(1 - dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(v - n*dt) - n*sqrt(discriminant);
        return true;
    }

    // Total internal reflection.
    return false;
}
