#ifndef HITABLE_H
#define HITABLE_H

#include "Ray.h"
#include "HitRecord.h"
#include "Aabb.h"

class Hitable {
public:
    // Must not touch rec if the ray misses or is outside the bounds.
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;

    // Set the aabb to the bounding box of this hitable over the time period. Returns
    // whether this hitable even has a bounding box.
    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const = 0;
};

#endif // HITABLE_H
