#ifndef BOX_H
#define BOX_H

#include "Hitable.h"
#include "HitableList.h"
#include "Rect.h"
#include "FlipNormals.h"

class Box : public Hitable {
public:
    Vec3 m_min;
    Vec3 m_max;
    Hitable *m_sides;

    Box(const Vec3 &min, const Vec3 &max, Material *material)
        : m_min(min), m_max(max) {

        Hitable **list = new Hitable*[6];
        int i = 0;
        list[i++] = new XyRect(min.x(), max.x(), min.y(), max.y(), max.z(), material);
        list[i++] = new FlipNormals(new XyRect(min.x(), max.x(), min.y(), max.y(), min.z(), material));
        list[i++] = new XzRect(min.x(), max.x(), min.z(), max.z(), max.y(), material);
        list[i++] = new FlipNormals(new XzRect(min.x(), max.x(), min.z(), max.z(), min.y(), material));
        list[i++] = new YzRect(min.y(), max.y(), min.z(), max.z(), max.x(), material);
        list[i++] = new FlipNormals(new YzRect(min.y(), max.y(), min.z(), max.z(), min.x(), material));
        m_sides = new HitableList(list, i);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        return m_sides->hit(r, t_min, t_max, rec);
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = Aabb(m_min, m_max);
        return true;
    }
};

#endif // BOX_H
