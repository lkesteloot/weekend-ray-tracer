#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "Material.h"

class Lambertian : public Material {
public:
    // Our color.
    Vec3 m_albedo;

    Lambertian(const Vec3 &albedo)
        : m_albedo(albedo) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        Vec3 target = rec.p + rec.n + random_in_unit_sphere();

        ray_out = Ray(rec.p, target - rec.p);
        attenuation = m_albedo;

        return true;
    }
};

#endif // LAMBERTIAN_H
