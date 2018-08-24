#ifndef HITABLE_H
#define HITABLE_H

#include "Ray.h"
#include "HitRecord.h"

class Hitable {
public:
    // Must not touch rec if the ray misses or is outside the bounds.
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

#endif // HITABLE_H
