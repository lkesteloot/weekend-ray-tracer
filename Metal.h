#ifndef METAL_H
#define METAL_H

#include "Material.h"

class Metal : public Material {
public:
    // Our color.
    Vec3 m_albedo;

    Metal(const Vec3 &albedo)
        : m_albedo(albedo) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        Vec3 reflected = reflect(ray_in.direction().unit(), rec.n);

        ray_out = Ray(rec.p, reflected);
        attenuation = m_albedo;

        return reflected.dot(rec.n) > 0;
    }
};

#endif // METAL_H
