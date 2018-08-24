#ifndef RAY_H
#define RAY_H

#include "Vec3.h"

/**
 * A ray with an origin and direction. The direction is not
 * necessarily of unit length.
 */
class Ray {
public:
    Vec3 mOrigin;
    Vec3 mDirection;

    Ray() {
        // Nothing.
    }
    Ray(const Vec3 &origin, const Vec3 &direction)
        : mOrigin(origin), mDirection(direction) {

        // Nothing.
    }
    const Vec3 &origin() const { return mOrigin; }
    const Vec3 &direction() const { return mDirection; }
    Vec3 point_at(float t) const { return mOrigin + t*mDirection; }
};

#endif // RAY_H
