#ifndef ISOTROPIC_H
#define ISOTROPIC_H

#include "Material.h"

// Material that reflects equally in all directions.
class Isotropic : public Material {
public:
    Texture *m_texture;

    Isotropic(Texture *texture)
        : m_texture(texture) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        ray_out = Ray(rec.p, random_in_unit_sphere(), ray_in.time());
        attenuation = m_texture->value(rec.u, rec.v, rec.p);

        return true;
    }
};

#endif // ISOTROPIC_H
