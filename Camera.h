#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

class Camera {
public:
    Vec3 m_origin;
    Vec3 m_lower_left_corner;
    Vec3 m_horizontal;
    Vec3 m_vertical;

    // vfov is vertical in degrees.
    // aspect is width to height;
    Camera(Vec3 look_from, Vec3 look_at, Vec3 vup, float vfov, float aspect) {
        // Axes of camera in world space.
        Vec3 x, y, z;

        // Vertical fov in radians.
        float theta = vfov*M_PI/180;

        float half_height = tan(theta/2);
        float half_width = aspect*half_height;

        m_origin = look_from;
        z = (look_from - look_at).unit();
        x = vup.cross(z).unit();
        y = z.cross(x);

        m_lower_left_corner = m_origin - half_width*x - half_height*y - z;
        m_horizontal = 2*half_width*x;
        m_vertical = 2*half_height*y;
    }

    Ray get_ray(float u, float v) {
        return Ray(m_origin, m_lower_left_corner + u*m_horizontal + v*m_vertical - m_origin);
    }
};

#endif // CAMERA_H
