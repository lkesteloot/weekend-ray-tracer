#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

class Camera {
public:
    Vec3 m_origin;
    Vec3 m_lower_left_corner;
    Vec3 m_horizontal;
    Vec3 m_vertical;

    Camera() {
        m_lower_left_corner = Vec3(-2.0, -1.0, -1.0);
        m_horizontal = Vec3(4.0, 0.0, 0.0);
        m_vertical = Vec3(0.0, 2.0, 0.0);
        m_origin = Vec3(0.0, 0.0, 0.0);
    }

    Ray get_ray(float u, float v) {
        return Ray(m_origin, m_lower_left_corner + u*m_horizontal + v*m_vertical - m_origin);
    }
};

#endif // CAMERA_H
