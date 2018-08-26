#ifndef RECT_H
#define RECT_H

#include "Hitable.h"
#include "Material.h"

#define AABB_EPSILON 0.0001

// Axis-aligned rectangle in XY plane.
class XyRect : public Hitable {
public:
    float m_x0, m_x1;
    float m_y0, m_y1;
    float m_z;
    Material *m_material;

    XyRect(float x0, float x1, float y0, float y1, float z, Material *material)
        : m_x0(x0), m_x1(x1),
          m_y0(y0), m_y1(y1),
          m_z(z), m_material(material) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        // Divide-by-zero is okay, it fails the next text.
        float t = (m_z - r.origin().z()) / r.direction().z();
        if (t < t_min || t > t_max) {
            // Outside ray bounds.
            return false;
        }

        Vec3 p = r.point_at(t);
        if (p.x() < m_x0 || p.x() > m_x1 || p.y() < m_y0 || p.y() > m_y1) {
            // Outside rectangle.
            return false;
        }

        rec.u = (p.x() - m_x0) / (m_x1 - m_x0);
        rec.v = (p.y() - m_y0) / (m_y1 - m_y0);
        rec.t = t;
        rec.material = m_material;
        rec.p = p;
        rec.n = Vec3(0, 0, 1);

        return true;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = Aabb(Vec3(m_x0, m_y0, m_z - AABB_EPSILON), Vec3(m_x1, m_y1, m_z + AABB_EPSILON));
        return true;
    }
};

// Axis-aligned rectangle in YZ plane.
class YzRect : public Hitable {
public:
    float m_y0, m_y1;
    float m_z0, m_z1;
    float m_x;
    Material *m_material;

    YzRect(float y0, float y1, float z0, float z1, float x, Material *material)
        : m_y0(y0), m_y1(y1),
          m_z0(z0), m_z1(z1),
          m_x(x), m_material(material) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        // Divide-by-zero is okay, it fails the next text.
        float t = (m_x - r.origin().x()) / r.direction().x();
        if (t < t_min || t > t_max) {
            // Outside ray bounds.
            return false;
        }

        Vec3 p = r.point_at(t);
        if (p.y() < m_y0 || p.y() > m_y1 || p.z() < m_z0 || p.z() > m_z1) {
            // Outside rectangle.
            return false;
        }

        rec.u = (p.y() - m_y0) / (m_y1 - m_y0);
        rec.v = (p.z() - m_z0) / (m_z1 - m_z0);
        rec.t = t;
        rec.material = m_material;
        rec.p = p;
        rec.n = Vec3(1, 0, 0);

        return true;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = Aabb(Vec3(m_x - AABB_EPSILON, m_y0, m_z0), Vec3(m_x + AABB_EPSILON, m_y1, m_z1));
        return true;
    }
};

// Axis-aligned rectangle in XZ plane.
class XzRect : public Hitable {
public:
    float m_x0, m_x1;
    float m_z0, m_z1;
    float m_y;
    Material *m_material;

    XzRect(float x0, float x1, float z0, float z1, float y, Material *material)
        : m_x0(x0), m_x1(x1),
          m_z0(z0), m_z1(z1),
          m_y(y), m_material(material) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        // Divide-by-zero is okay, it fails the next text.
        float t = (m_y - r.origin().y()) / r.direction().y();
        if (t < t_min || t > t_max) {
            // Outside ray bounds.
            return false;
        }

        Vec3 p = r.point_at(t);
        if (p.x() < m_x0 || p.x() > m_x1 || p.z() < m_z0 || p.z() > m_z1) {
            // Outside rectangle.
            return false;
        }

        rec.u = (p.x() - m_x0) / (m_x1 - m_x0);
        rec.v = (p.z() - m_z0) / (m_z1 - m_z0);
        rec.t = t;
        rec.material = m_material;
        rec.p = p;
        rec.n = Vec3(0, 1, 0);

        return true;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = Aabb(Vec3(m_x0, m_y - AABB_EPSILON, m_z0), Vec3(m_x1, m_y + AABB_EPSILON, m_z1));
        return true;
    }
};

#endif // RECT_H
