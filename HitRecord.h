#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "Vec3.h"

struct HitRecord {
    // Distance along the ray.
    float t;

    // Intersection point.
    Vec3 p;

    // Normal, always normalized.
    Vec3 n;
};

#endif // HIT_RECORD_H
