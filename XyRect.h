#ifndef XY_RECT_H
#define XY_RECT_H

#include "Hitable.h"
#include "Material.h"

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
        aabb = Aabb(Vec3(m_x0, m_y0, m_z - 0.0001), Vec3(m_x1, m_y1, m_z + 0.0001));
        return true;
    }
};

#endif // XY_RECT_H
