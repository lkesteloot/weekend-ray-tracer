#ifndef METAL_H
#define METAL_H

#include "Material.h"

class Metal : public Material {
public:
    // Our color.
    Vec3 m_albedo;

    // How much to permute the reflection.
    float m_fuzz;

    Metal(const Vec3 &albedo, float fuzz)
        : m_albedo(albedo), m_fuzz(fuzz) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        Vec3 reflected = reflect(ray_in.direction().unit(), rec.n);

        if (m_fuzz > 0) {
            reflected += m_fuzz*random_in_unit_sphere();
        }

        ray_out = Ray(rec.p, reflected, ray_in.time());
        attenuation = m_albedo;

        return reflected.dot(rec.n) > 0;
    }
};

#endif // METAL_H
