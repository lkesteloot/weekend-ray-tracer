#include <iostream>
#include <float.h>
#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"

static Vec3 random_in_unit_sphere() {
    Vec3 p;

    do {
        p = 2.0*Vec3(drand48(), drand48(), drand48()) - VEC3_ONES;
    } while (p.squared_length() > 1.0);

    return p;
}

static Vec3 color(const Ray &r, Hitable *world) {
    HitRecord rec;

    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        Vec3 target = rec.p + rec.n + random_in_unit_sphere();
        return 0.5*color(Ray(rec.p, target - rec.p), world);
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

            // Gamma correct.
            c = Vec3(sqrt(c.r()), sqrt(c.g()), sqrt(c.b()));

            int ir = int(255.99*c.r());
            int ig = int(255.99*c.g());
            int ib = int(255.99*c.b());
            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    return 0;
}
