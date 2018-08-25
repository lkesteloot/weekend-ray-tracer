#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "Vec3.h"

class Material;

struct HitRecord {
    // Distance along the ray.
    float t;

    // Intersection point.
    Vec3 p;

    // Normal, always normalized.
    Vec3 n;

    // Material we hit.
    Material *material;
};

#endif // HIT_RECORD_H
