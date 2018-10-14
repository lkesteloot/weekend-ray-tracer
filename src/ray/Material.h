#ifndef MATERIAL_H
#define MATERIAL_H

#include "Ray.h"
#include "HitRecord.h"

// Base class for materials.
class Material {
public:
    // Returns whether to follow ray_out.
    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const = 0;

    // Return the color emitted at this point.
    virtual Vec3 emitted(float /* u */, float /* v */, const Vec3 & /* p */) const {
        return Vec3(0, 0, 0);
    }
};

#endif // MATERIAL_H
