#ifndef SPHERE_H
#define SPHERE_H

#include "Hitable.h"

class Sphere : public Hitable {
public:
    Vec3 mCenter;
    float mRadius;

    Sphere() {
        // Nothing.
    }
    Sphere(const Vec3 center, float radius)
        : mCenter(center), mRadius(radius) {

        // Nothing.
    }

    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const {
        Vec3 oc = r.origin() - mCenter;

        // Note that we halve B here, but compensate later:
        float a = r.direction().dot(r.direction());
        float b = oc.dot(r.direction()); // Should be twice this.
        float c = oc.dot(oc) - mRadius*mRadius;
        // Remove 4* before a*c, so this discriminant is one fourth what it should be.
        float discriminant = b*b - a*c;

        if (discriminant >= 0) {
            // Remove doubling of A to compensate for numerator being half its real value.
            float t = (-b - sqrt(discriminant))/a;
            if (t < t_max && t > t_min) {
                rec.t = t;
                rec.p = r.point_at(rec.t);
                rec.n = (rec.p - mCenter) / mRadius;
                return true;
            }
            // Remove doubling of A to compensate for numerator being half its real value.
            t = (-b + sqrt(discriminant))/a;
            if (t < t_max && t > t_min) {
                rec.t = t;
                rec.p = r.point_at(rec.t);
                rec.n = (rec.p - mCenter) / mRadius;
                return true;
            }
        }
    
        return false;
    }
};

#endif // SPHERE_H
