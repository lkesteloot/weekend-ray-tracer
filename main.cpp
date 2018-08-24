#include <iostream>
#include "Vec3.h"
#include "Ray.h"

static Vec3 color(const Ray &r) {
    Vec3 unit_direction = r.direction().unit();
    float t = 0.5*(unit_direction.y() + 1.0);

    return (1.0 - t)*Vec3(1.0, 1.0, 1.0) + t*Vec3(0.5, 0.7, 1.0);
}

int main() {
    int nx = 200;
    int ny = 100;

    Vec3 lower_left_corner(-2.0, 1.0, -1.0);
    Vec3 horizontal(4.0, 0.0, 0.0);
    Vec3 vertical(0.0, 2.0, 0.0);
    Vec3 origin(0.0, 0.0, 0.0);

    std::cout << "P3 " << nx << " " << ny << " 255\n";
    for (int j = ny - 1; j >= 0; j--) {
        float v = float(j)/ny;

        for (int i = nx - 1; i >= 0; i--) {
            float u = float(i)/nx;

            Ray r(origin, lower_left_corner + u*horizontal + v*vertical);
            Vec3 c = color(r);

            int ir = int(255.99*c.r());
            int ig = int(255.99*c.g());
            int ib = int(255.99*c.b());
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    return 0;
}
