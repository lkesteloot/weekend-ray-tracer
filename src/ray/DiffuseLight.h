#ifndef DIFFUSE_LIGHT_H
#define DIFFUSE_LIGHT_H

#include "Material.h"
#include "Texture.h"

class DiffuseLight : public Material {
public:
    Texture *m_emit;

    DiffuseLight(Texture *emit)
        : m_emit(emit) {

        // Nothing.
    }

    virtual bool scatter(const Ray &, const HitRecord &, Vec3 &, Ray &) const {
        // Not scattering.
        return false;
    }

    virtual Vec3 emitted(float u, float v, const Vec3 &p) const {
        // Defer to texture.
        return m_emit->value(u, v, p);
    }
};

#endif // DIFFUSE_LIGHT_H
