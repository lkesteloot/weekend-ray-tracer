#include <iostream>
#include <float.h>

#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

static Vec3 color(const Ray &r, Hitable *world, int depth) {
    HitRecord rec;

    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        Ray scattered;
        Vec3 attenuation;

        if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered)) {
            return attenuation*color(scattered, world, depth + 1);
        } else {
            return VEC3_BLACK;
        }
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

    Hitable *list[4];
    list[0] = new Sphere(Vec3(0, 0, -1), 0.5, new Lambertian(Vec3(0.8, 0.3, 0.3)));
    list[1] = new Sphere(Vec3(0, -100.5, -1), 100, new Lambertian(Vec3(0.8, 0.8, 0.0)));
    list[2] = new Sphere(Vec3(1, 0, -1), 0.5, new Metal(Vec3(0.8, 0.6, 0.2), 0.3));
    list[3] = new Sphere(Vec3(-1, 0, -1), 0.5, new Dielectric(REF_GLASS));
    Hitable *world = new HitableList(list, 4);

    std::cout << "P3 " << nx << " " << ny << " 255\n";
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            Vec3 c(0, 0, 0);
            for (int s = 0; s < ns; s++) {
                float u = (i + drand48())/nx;
                float v = (j + drand48())/ny;

                Ray r = cam.get_ray(u, v);
                c += color(r, world, 0);
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
