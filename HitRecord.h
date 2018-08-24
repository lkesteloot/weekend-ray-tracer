#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include "Vec3.h"

struct HitRecord {
    float t;
    Vec3 p;
    Vec3 n;
};

#endif // HIT_RECORD_H
