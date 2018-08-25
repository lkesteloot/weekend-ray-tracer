#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

class Camera {
public:
    float m_lens_radius;
    Vec3 m_origin;
    Vec3 m_lower_left_corner;
    Vec3 m_horizontal;
    Vec3 m_vertical;
    // Axes of camera in world space.
    Vec3 m_x;
    Vec3 m_y;
    Vec3 m_z;

    // vfov is vertical in degrees.
    // aspect is width to height;
    Camera(Vec3 look_from, Vec3 look_at, Vec3 vup, float vfov, float aspect,
            float aperature, float focus_distance) {

        m_lens_radius = aperature/2;

        // Vertical fov in radians.
        float theta = vfov*M_PI/180;

        float half_height = tan(theta/2);
        float half_width = aspect*half_height;

        m_origin = look_from;
        m_z = (look_from - look_at).unit();
        m_x = vup.cross(m_z).unit();
        m_y = m_z.cross(m_x);

        m_lower_left_corner = m_origin - focus_distance*(half_width*m_x + half_height*m_y + m_z);
        m_horizontal = 2*half_width*focus_distance*m_x;
        m_vertical = 2*half_height*focus_distance*m_y;
    }

    Ray get_ray(float u, float v) const {
        // Compute random displacement.
        Vec3 rd;
        if (m_lens_radius == 0) {
            rd = VEC3_ORIGIN;
        } else {
            rd = m_lens_radius*random_in_unit_disc();
        }

        // Displacement in world space.
        Vec3 offset = m_x*rd.x() + m_y*rd.y();
        return Ray(m_origin + offset,
                m_lower_left_corner + u*m_horizontal + v*m_vertical - m_origin - offset);
    }
};

#endif // CAMERA_H
