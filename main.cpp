#include <iostream>
#include <float.h>
#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"

static Vec3 color(const Ray &r, Hitable *world) {
    HitRecord rec;

    if (world->hit(r, 0.0, MAXFLOAT, rec)) {
        return 0.5*Vec3(rec.n.x() + 1, rec.n.y() + 1, rec.n.z() + 1);
    } else {
        // Sky background.
        Vec3 unit_direction = r.direction().unit();
        float t = 0.5*(unit_direction.y() + 1.0);

        return (1.0 - t)*Vec3(1.0, 1.0, 1.0) + t*Vec3(0.5, 0.7, 1.0);
    }
}

int main() {
    int nx = 200*4;
    int ny = 100*4;
    int ns = 100;

    Camera cam;

    Hitable *list[2];
    list[0] = new Sphere(Vec3(0, 0, -1), 0.5);
    list[1] = new Sphere(Vec3(0, -100.5, -1), 100);
    Hitable *world = new HitableList(list, 2);

    std::cout << "P3 " << nx << " " << ny << " 255\n";
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = nx - 1; i >= 0; i--) {
            Vec3 c(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = (i + drand48())/nx;
                float v = (j + drand48())/ny;

                Ray r = cam.get_ray(u, v);
                c += color(r, world);
            }
            c /= ns;

            int ir = int(255.99*c.r());
            int ig = int(255.99*c.g());
            int ib = int(255.99*c.b());
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    return 0;
}
