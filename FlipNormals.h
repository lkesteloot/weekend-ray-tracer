#ifndef FLIP_NORMALS_H
#define FLIP_NORMALS_H

#include "Hitable.h"

class FlipNormals : public Hitable {
public:
    Hitable *m_hitable;

    FlipNormals(Hitable *hitable)
        : m_hitable(hitable) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        if (m_hitable->hit(r, t_min, t_max, rec)) {
            rec.n = -rec.n;
            return true;
        }

        return false;
    }

    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const {
        return m_hitable->bounding_box(t0, t1, aabb);
    }
};

#endif // FLIP_NORMALS_H
