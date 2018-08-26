#ifndef SPHERE_H
#define SPHERE_H

#include "Hitable.h"

// Convert normalized point on sphere to polar coordinates.
static void get_sphere_uv(const Vec3 &p, float &u, float &v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());

    u = 1 - (phi + M_PI) / (2*M_PI);
    v = (theta + M_PI/2) / M_PI;
}

class Sphere : public Hitable {
public:
    Vec3 m_center;
    float m_radius;
    Material *m_material;

    Sphere() {
        // Nothing.
    }
    Sphere(const Vec3 center, float radius, Material *material)
        : m_center(center), m_radius(radius), m_material(material) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        Vec3 oc = r.origin() - m_center;

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
                rec.n = (rec.p - m_center) / m_radius;
                rec.material = m_material;
                get_sphere_uv(rec.n, rec.u, rec.v);
                return true;
            }
            // Remove doubling of A to compensate for numerator being half its real value.
            t = (-b + sqrt(discriminant))/a;
            if (t < t_max && t > t_min) {
                rec.t = t;
                rec.p = r.point_at(rec.t);
                rec.n = (rec.p - m_center) / m_radius;
                rec.material = m_material;
                get_sphere_uv(rec.n, rec.u, rec.v);
                return true;
            }
        }
    
        return false;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        Vec3 radius = Vec3(m_radius, m_radius, m_radius);
        aabb = Aabb(m_center - radius, m_center + radius);
        return true;
    }
};

#endif // SPHERE_H
