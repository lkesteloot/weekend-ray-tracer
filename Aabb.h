#ifndef AABB_H
#define AABB_H

// Axis-aligned bounding box.
class Aabb {
public:
    Vec3 m_min;
    Vec3 m_max;

    Aabb() {
        // Nothing.
    }
    Aabb(const Vec3 &min, const Vec3 &max)
        : m_min(min), m_max(max) {

        // Nothing.
    }

    inline const Vec3 &min() const { return m_min; }
    inline const Vec3 &max() const { return m_max; }

    bool hit(const Ray &ray, float t_min, float t_max) const {
        Vec3 origin = ray.origin();
        Vec3 direction = ray.direction();

        for (int a = 0; a < 3; a++) {
            // Compute the two endpoints for this dimension. Division
            // by zero here is okay, it'll give infinity, which will
            // do the right thing later.
            float inv_d = 1/direction[a];
            float t0 = (m_min[a] - origin[a])*inv_d;
            float t1 = (m_max[a] - origin[a])*inv_d;

            // Make t0 <= t1.
            if (inv_d < 0) {
                std::swap(t0, t1);
            }

            // Keep intersecting.
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;

            // If empty set, then we don't hit.
            if (t_max <= t_min) {
                return false;
            }
        }

        return true;
    }

    Aabb union_with(const Aabb &o) const {
        return Aabb(m_min.min(o.m_min), m_max.max(o.m_max));
    }

};

#endif // AABB_H
