#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Hitable.h"
#include "Texture.h"

// Translates another hitable.
class Translate : public Hitable {
public:
    Hitable *m_hitable;
    Vec3 m_offset;

    Translate(Hitable *hitable, const Vec3 &offset)
        : m_hitable(hitable), m_offset(offset) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        Ray translated_ray(r.origin() - m_offset, r.direction(), r.time());

        if (m_hitable->hit(translated_ray, t_min, t_max, rec)) {
            rec.p += m_offset;
            return true;
        }

        return false;
    }

    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const {
        if (m_hitable->bounding_box(t0, t1, aabb)) {
            aabb = Aabb(aabb.min() + m_offset, aabb.max() + m_offset);
            return true;
        }

        return false;
    }
};

// Rotates another hitable.
class RotateY : public Hitable {
public:
    Hitable *m_hitable;
    float m_sin_theta;
    float m_cos_theta;
    bool m_has_aabb;
    Aabb m_aabb;

    // Angle is in degrees.
    RotateY(Hitable *hitable, float angle)
        : m_hitable(hitable) {

        // Convert to radians.
        float theta = angle*M_PI/180;

        // Precompute these.
        m_sin_theta = sin(theta);
        m_cos_theta = cos(theta);

        // Get the sub-object's bounding box. Note that the time here is
        // bogus, we should use the times when we get called in bounding_box().
        m_has_aabb = m_hitable->bounding_box(0, 1, m_aabb);

        // Rotate bounding box.
        Vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
        Vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        // Try every corner.
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    // Get corner.
                    float x = i*m_aabb.max().x() + (1 - i)*m_aabb.min().x();
                    float y = j*m_aabb.max().y() + (1 - j)*m_aabb.min().y();
                    float z = k*m_aabb.max().z() + (1 - k)*m_aabb.min().z();

                    // Rotate corner.
                    float new_x = m_cos_theta*x + m_sin_theta*z;
                    float new_z = -m_sin_theta*x + m_cos_theta*z;

                    // Stretch out new box.
                    Vec3 new_corner(new_x, y, new_z);
                    min = min.min(new_corner);
                    max = max.max(new_corner);
                }
            }
        }

        m_aabb = Aabb(min, max);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        // Rotate our incoming ray.
        Vec3 origin(
                m_cos_theta*r.origin().x() - m_sin_theta*r.origin().z(),
                r.origin().y(),
                m_sin_theta*r.origin().x() + m_cos_theta*r.origin().z());
        Vec3 direction(
                m_cos_theta*r.direction().x() - m_sin_theta*r.direction().z(),
                r.direction().y(),
                m_sin_theta*r.direction().x() + m_cos_theta*r.direction().z());

        // Make a new ray.
        Ray rotated_ray(origin, direction, r.time());

        if (m_hitable->hit(rotated_ray, t_min, t_max, rec)) {
            // Rotate the hit back.
            Vec3 p(
                    m_cos_theta*rec.p.x() + m_sin_theta*rec.p.z(),
                    rec.p.y(),
                    -m_sin_theta*rec.p.x() + m_cos_theta*rec.p.z());
            Vec3 n(
                    m_cos_theta*rec.n.x() + m_sin_theta*rec.n.z(),
                    rec.n.y(),
                    -m_sin_theta*rec.n.x() + m_cos_theta*rec.n.z());

            rec.p = p;
            rec.n = n;
            return true;
        }

        return false;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = m_aabb;
        return m_has_aabb;
    }
};

// Transforms a texture.
class TransformTexture : public Texture {
public:
    Texture *m_texture;
    Vec3 m_translate;

    TransformTexture(Texture *texture, const Vec3 &translate)
        : m_texture(texture), m_translate(translate) {

        // Nothing.
    }

    virtual Vec3 value(float u, float v, const Vec3 &p) const {
        Vec3 new_p = p - m_translate;

        return m_texture->value(u, v, new_p);
    }
};

#endif // TRANSFORM_H
