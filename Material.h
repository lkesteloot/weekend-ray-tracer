#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "HitRecord.h"

class Material {
public:
    // Returns whether to follow ray_out.
    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const = 0;
};

#endif // MATERIAL_H
