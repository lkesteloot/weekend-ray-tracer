#include <iostream>
#include <float.h>
#include <thread>

#include "Sphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

static const int WIDTH = 200*4;
static const int HEIGHT = 100*4;
static const int STRIDE = WIDTH*3;
static const int SAMPLE_COUNT = 10;
static const int THREAD_COUNT = 8;

static Hitable *random_scene() {
    int n = 500;

    Hitable **list = new Hitable*[n + 1];

    int i = 0;

    // Ground.
    list[i++] = new Sphere(Vec3(0, -1000, 0), 1000, new Lambertian(Vec3(0.5, 0.5, 0.5)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();

            Vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9) {
                Material *material;

                if (choose_mat < 0.8) {
                    // Diffuse.
                    material = new Lambertian(Vec3(
                                drand48()*drand48(),
                                drand48()*drand48(),
                                drand48()*drand48()));
                } else if (choose_mat < 0.95) {
                    // Metal.
                    material = new Metal(Vec3(
                                0.5*(1 + drand48()),
                                0.5*(1 + drand48()),
                                0.5*(1 + drand48())),
                            0.5*drand48());
                } else {
                    // Glass.
                    material = new Dielectric(REF_GLASS);
                }

                list[i++] = new Sphere(center, 0.2, material);
            }
        }
    }

    // Large spheres.
    list[i++] = new Sphere(Vec3(0, 1, 0), 1.0, new Dielectric(REF_GLASS));
    list[i++] = new Sphere(Vec3(-4, 1, 0), 1.0, new Lambertian(Vec3(0.4, 0.2, 0.1)));
    list[i++] = new Sphere(Vec3(4, 1, 0), 1.0, new Metal(Vec3(0.7, 0.6, 0.5), 0.0));

    return new HitableList(list, i);
}

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

// Trace line "j".
static void trace_line(unsigned char *row, int j, const Camera &cam, Hitable *world) {
    for (int i = 0; i < WIDTH; i++) {
        // Oversample.
        Vec3 c(0, 0, 0);
        for (int s = 0; s < SAMPLE_COUNT; s++) {
            float u = (i + drand48())/WIDTH;
            float v = (j + drand48())/HEIGHT;

            Ray r = cam.get_ray(u, v);
            c += color(r, world, 0);
        }
        c /= SAMPLE_COUNT;

        // Gamma correct.
        c = Vec3(sqrt(c.r()), sqrt(c.g()), sqrt(c.b()));

        int ir = int(255.99*c.r());
        int ig = int(255.99*c.g());
        int ib = int(255.99*c.b());
        row[0] = ir;
        row[1] = ig;
        row[2] = ib;
        row += 3;
    }
}

// Trace line "start" and every "skip" lines after that.
static void trace_lines(unsigned char *image, int start, int skip,
        const Camera &cam, Hitable *world) {

    for (int j = start; j < HEIGHT; j += skip) {
        unsigned char *row = image + j*STRIDE;

        trace_line(row, HEIGHT - 1 - j, cam, world);
    }
}

int main() {
    Vec3 look_from = Vec3(13, 2, 3);
    Vec3 look_at = Vec3(0, 0, 0);
    float focus_distance = 10;
    float aperature = 0.1;

    Camera cam(look_from, look_at, Vec3(0, 1, 0), 20, float(WIDTH)/HEIGHT,
            aperature, focus_distance);

    Hitable *world = random_scene();

    unsigned char *image = new unsigned char[STRIDE*HEIGHT];

    // Generate the image on multiple threads.
    std::thread *thread[THREAD_COUNT];
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t] = new std::thread(trace_lines, image, t, THREAD_COUNT, cam, world);
    }
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t]->join();
        delete thread[t];
        thread[t] = 0;
    }

    // Write image.
    std::cout << "P6 " << WIDTH << " " << HEIGHT << " 255\n";
    for (int i = 0; i < STRIDE*HEIGHT; i++) {
        std::cout << image[i];
    }

    return 0;
}
