#ifndef MOVING_SPHERE_H
#define MOVING_SPHERE_H

#include "Hitable.h"

class MovingSphere : public Hitable {
public:
    Vec3 m_initial_center, m_delta_center;
    float m_initial_time, m_delta_time;
    float m_radius;
    Material *m_material;

    MovingSphere() {
        // Nothing.
    }
    MovingSphere(const Vec3 &center0, const Vec3 &center1,
            float time0, float time1,
            float radius, Material *material)
        : m_initial_center(center0), m_delta_center(center1 - center0),
          m_initial_time(time0), m_delta_time(time1 - time0),
          m_radius(radius), m_material(material) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        Vec3 center = center_at_time(r.time());

        // Move ray to pretend sphere is at origin.
        Vec3 oc = r.origin() - center;

        // Note that we halve B here, but compensate later:
        float a = r.direction().dot(r.direction());
        float b = oc.dot(r.direction()); // Should be twice this.
        float c = oc.dot(oc) - m_radius*m_radius;
        // Remove 4* before a*c, so this discriminant is one fourth what it should be.
        float discriminant = b*b - a*c;

        if (discriminant >= 0) {
            // Remove doubling of A to compensate for numerator being half its real value.
            float t = (-b - sqrt(discriminant))/a;
            if (t < t_max && t > t_min) {
                rec.t = t;
                rec.p = r.point_at(rec.t);
                rec.n = (rec.p - center) / m_radius;
                rec.material = m_material;
                return true;
            }
            // Remove doubling of A to compensate for numerator being half its real value.
            t = (-b + sqrt(discriminant))/a;
            if (t < t_max && t > t_min) {
                rec.t = t;
                rec.p = r.point_at(rec.t);
                rec.n = (rec.p - center) / m_radius;
                rec.material = m_material;
                return true;
            }
        }
    
        return false;
    }

    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const {
        // Union of AABBs of both spheres.
        Vec3 radius = Vec3(m_radius, m_radius, m_radius);
        Aabb aabb0 = Aabb(center_at_time(t0) - radius, center_at_time(t0) + radius);
        Aabb aabb1 = Aabb(center_at_time(t1) - radius, center_at_time(t1) + radius);

        aabb = aabb0.union_with(aabb1);

        return true;
    }

private:
    Vec3 center_at_time(float time) const {
        return m_initial_center + (time - m_initial_time)/m_delta_time*m_delta_center;
    }
};

#endif // MOVING_SPHERE_H
