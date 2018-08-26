#ifndef LAMBERTIAN_H
#define LAMBERTIAN_H

#include "Material.h"
#include "Texture.h"

class Lambertian : public Material {
public:
    Texture *m_texture;

    Lambertian(Texture *texture)
        : m_texture(texture) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        Vec3 target = rec.p + rec.n + random_in_unit_sphere();

        ray_out = Ray(rec.p, target - rec.p, ray_in.time());
        attenuation = m_texture->value(rec.u, rec.v, rec.p);

        return true;
    }
};

#endif // LAMBERTIAN_H
