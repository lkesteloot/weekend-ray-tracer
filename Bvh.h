#ifndef BVH_H
#define BVH_H

#include "Hitable.h"

// For qsort().
int aabb_x_compare(const void *a, const void *b) {
    Hitable *ah = *(Hitable **) a;
    Hitable *bh = *(Hitable **) b;

    Aabb aabb_a, aabb_b;

    // Okay to use times 0,0 here, it's only for sorting.
    if (!ah->bounding_box(0, 0, aabb_a) ||
        !bh->bounding_box(0, 0, aabb_b)) {

        std::cerr << "No bounding box for qsort.\n";
    }

    if (aabb_a.min().x() < aabb_b.min().x()) {
        return -1;
    } else {
        return 0;
    }
}

// For qsort().
int aabb_y_compare(const void *a, const void *b) {
    Hitable *ah = *(Hitable **) a;
    Hitable *bh = *(Hitable **) b;

    Aabb aabb_a, aabb_b;

    // Okay to use times 0,0 here, it's only for sorting.
    if (!ah->bounding_box(0, 0, aabb_a) ||
        !bh->bounding_box(0, 0, aabb_b)) {

        std::cerr << "No bounding box for qsort.\n";
    }

    if (aabb_a.min().y() < aabb_b.min().y()) {
        return -1;
    } else {
        return 0;
    }
}

// For qsort().
int aabb_z_compare(const void *a, const void *b) {
    Hitable *ah = *(Hitable **) a;
    Hitable *bh = *(Hitable **) b;

    Aabb aabb_a, aabb_b;

    // Okay to use times 0,0 here, it's only for sorting.
    if (!ah->bounding_box(0, 0, aabb_a) ||
        !bh->bounding_box(0, 0, aabb_b)) {

        std::cerr << "No bounding box for qsort.\n";
    }

    if (aabb_a.min().z() < aabb_b.min().z()) {
        return -1;
    } else {
        return 0;
    }
}


// Bounding volume hierarchy node.
class Bvh : public Hitable {
public:
    // Neither of these are ever null.
    Hitable *m_left;
    Hitable *m_right;
    Aabb m_aabb;

    Bvh(Hitable **list, int size, float t0, float t1) {
        // Pick random axis to sort on.
        int axis = int(3*my_rand());
        switch (axis) {
            case 0:
                qsort(list, size, sizeof(Hitable *), aabb_x_compare);
                break;

            case 1:
                qsort(list, size, sizeof(Hitable *), aabb_y_compare);
                break;

            case 2:
                qsort(list, size, sizeof(Hitable *), aabb_z_compare);
                break;
        }

        if (size == 0) {
            std::cerr << "We don't handle zero items in BVH.\n";
        } else if (size == 1) {
            // Wasteful, but only happens if we have one object in the entire scene.
            m_left = list[0];
            m_right = list[0];
        } else if (size == 2) {
            m_left = list[0];
            m_right = list[1];
        } else if (size == 3) {
            // Special case to avoid needless box with one item.
            m_left = new Bvh(list, 2, t0, t1);
            m_right = list[2];
        } else {
            int half = size/2;
            m_left = new Bvh(list, half, t0, t1);
            m_right = new Bvh(list + half, size - half, t0, t1);
        }

        Aabb aabb_left;
        Aabb aabb_right;

        if (!m_left->bounding_box(t0, t1, aabb_left) ||
            !m_right->bounding_box(t0, t1, aabb_right)) {

            std::cerr << "No bounding box in BVH constructor.\n";
        }

        m_aabb = aabb_left.union_with(aabb_right);
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        if (m_aabb.hit(r, t_min, t_max)) {
            HitRecord left_rec, right_rec;

            bool hit_left = m_left->hit(r, t_min, t_max, left_rec);
            if (hit_left) {
                // If we've hit something on the left, there's no need to look
                // past that when looking on the right.
                bool hit_right = m_right->hit(r, t_min, left_rec.t, right_rec);
                if (hit_right) {
                    rec = right_rec;
                } else {
                    rec = left_rec;
                }
                return true;
            }

            bool hit_right = m_right->hit(r, t_min, t_max, right_rec);
            if (hit_right) {
                rec = right_rec;
            }

            return hit_right;
        }

        return false;
    }

    virtual bool bounding_box(float, float, Aabb &aabb) const {
        aabb = m_aabb;
        return true;
    }
};

#endif // BVH_H
