#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include <cfloat>

#include "Hitable.h"
#include "Material.h"
#include "Isotropic.h"

class ConstantMedium : public Hitable {
public:
    Hitable *m_boundary;
    float m_density;
    Material *m_phase_function;

    ConstantMedium(Hitable *boundary, float density, Texture *texture)
        : m_boundary(boundary), m_density(density) {

        m_phase_function = new Isotropic(texture);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        HitRecord rec1, rec2;

        // Find the next and far sides of the boundary.
        if (m_boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
            if (m_boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2)) {
                // Clip to allowed t values.
                if (rec1.t < t_min) {
                    rec1.t = t_min;
                }
                if (rec2.t > t_max) {
                    rec2.t = t_max;
                }
                // If empty set, we don't intersect volume at all.
                if (rec1.t >= rec2.t) {
                    return false;
                }
                // Can't start behind us.
                if (rec1.t < 0) {
                    rec1.t = 0;
                }

                // Unit that "t" is expressed in.
                float direction_length = r.direction().length();

                // World distance within volume.
                float distance_inside_boundary = (rec2.t - rec1.t)*direction_length;

                // How far we went into it.
                float hit_distance = -(1/m_density)*log(my_rand());

                // See if we stopped inside it.
                if (hit_distance < distance_inside_boundary) {
                    // Back to "t" space.
                    rec.t = rec1.t + hit_distance/direction_length;
                    rec.p = r.point_at(rec.t);
                    rec.n = Vec3(1, 0, 0); // Ignored later.
                    rec.material = m_phase_function;
                    return true;
                }
            }
        }

        return false;
    }

    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const {
        return m_boundary->bounding_box(t0, t1, aabb);
    }
};

#endif // CONSTANT_MEDIUM_H
