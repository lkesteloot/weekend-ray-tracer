#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "Material.h"

const float REF_AIR = 1;
const float REF_GLASS = 1.5;  // 1.3 to 1.7
const float REF_DIAMOND = 2.4;

// Approximate reflection coefficient.
static float schlick(float cosine, float refraction_index) {
    float r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 *= r0;
    return r0 + (1 - r0)*pow(1 - cosine, 5);
}

// Dielectric material, like glass.
class Dielectric : public Material {
public:
    float m_refraction_index;

    Dielectric(float refraction_index)
        : m_refraction_index(refraction_index) {

        // Nothing.
    }

    virtual bool scatter(const Ray &ray_in, const HitRecord &rec,
            Vec3 &attenuation, Ray &ray_out) const {

        // Our ray's direction, normalized.
        Vec3 dir = ray_in.direction().unit();

        // No attenuation.
        attenuation = Vec3(1.0, 1.0, 1.0);

        // We don't know whether we're inside the material or outside.
        // Our hit normal will always point outward. We want a normal
        // that points in the direction we came from.
        Vec3 normal;
        float ni_over_nt;
        float cosine;
        if (dir.dot(rec.n) > 0) {
            // We're inside. Reverse normal.
            normal = -rec.n;
            ni_over_nt = m_refraction_index;
            cosine = m_refraction_index*dir.dot(rec.n);
        } else {
            // We're outside. Use normal as-is.
            normal = rec.n;
            ni_over_nt = 1/m_refraction_index;
            cosine = -dir.dot(rec.n);
        }

        Vec3 refracted;
        if (refract(dir, normal, ni_over_nt, refracted)) {
            // We can refract. Figure out if we should.
            float reflection_probability = schlick(cosine, m_refraction_index);

            if (drand48() < reflection_probability) {
                Vec3 reflected = reflect(dir, rec.n);
                ray_out = Ray(rec.p, reflected);
            } else {
                ray_out = Ray(rec.p, refracted);
            }
        } else {
            // Can't refract. Only reflect.
            Vec3 reflected = reflect(dir, rec.n);
            ray_out = Ray(rec.p, reflected);
        }

        return true;
    }
};

#endif // DIELECTRIC_H
