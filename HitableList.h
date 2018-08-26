#ifndef HITABLE_LIST_H
#define HITABLE_LIST_H

#include <vector>

#include "Hitable.h"

class HitableList : public Hitable {
public:
    std::vector<Hitable *> m_list;

    HitableList() {
        // Nothing.
    }

    void add(Hitable *hitable) {
        m_list.push_back(hitable);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        bool hit_anything = false;
        float closest = t_max;

        for (Hitable *hitable : m_list) {
            if (hitable->hit(r, t_min, closest, rec)) {
                hit_anything = true;
                closest = rec.t;
            }
        }

        return hit_anything;
    }

    virtual bool bounding_box(float t0, float t1, Aabb &aabb) const {
        if (m_list.empty()) {
            return false;
        }

        bool is_first = true;
        for (Hitable *hitable : m_list) {
            Aabb this_aabb;

            if (hitable->bounding_box(t0, t1, this_aabb)) {
                if (is_first) {
                    aabb = this_aabb;
                } else {
                    aabb = aabb.union_with(this_aabb);
                }
            } else {
                return false;
            }

            is_first = false;
        }

        return true;
    }
};


#endif // HITABLE_LIST_H
