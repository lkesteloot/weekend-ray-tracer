#include <iostream>
#include <float.h>
#include <thread>

#include "Sphere.h"
#include "MovingSphere.h"
#include "HitableList.h"
#include "Camera.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"
#include "Bvh.h"
#include "ConstantTexture.h"
#include "CheckerTexture.h"
#include "NoiseTexture.h"
#include "ImageTexture.h"
#include "DiffuseLight.h"
#include "Rect.h"
#include "FlipNormals.h"
#include "Box.h"
#include "Transform.h"
#include "ConstantMedium.h"

static const int WIDTH = 400;
static const int HEIGHT = 400;
static const int STRIDE = WIDTH*3;
static const int BYTE_COUNT = STRIDE*HEIGHT;
static const int SAMPLE_COUNT = 1000;
static const int THREAD_COUNT = 8;

/*
static Hitable *cornell_box() {
    Hitable **list = new Hitable*[8];

    Material *red = new Lambertian(new ConstantTexture(Vec3(0.65, 0.05, 0.05)));
    Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *green = new Lambertian(new ConstantTexture(Vec3(0.12, 0.45, 0.15)));
    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));

    int i = 0;
    list[i++] = new FlipNormals(new YzRect(0, 555, 0, 555, 555, green));    // Left
    list[i++] = new YzRect(0, 555, 0, 555, 0, red);                         // Right
    list[i++] = new XzRect(113, 443, 127, 432, 554, light);                 // Light
    list[i++] = new FlipNormals(new XzRect(0, 555, 0, 555, 555, white));    // Ceiling
    list[i++] = new XzRect(0, 555, 0, 555, 0, white);                       // Floor
    list[i++] = new FlipNormals(new XyRect(0, 555, 0, 555, 555, white));    // Back
    Hitable *b1 = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 165, 165), white),
                -18), Vec3(130, 0, 65));
    Hitable *b2 = new Translate(new RotateY(new Box(Vec3(0, 0, 0), Vec3(165, 330, 165), white),
                15), Vec3(265, 0, 295));
    list[i++] = new ConstantMedium(b1, 0.01, new ConstantTexture(Vec3(1, 1, 1)));
    list[i++] = new ConstantMedium(b2, 0.01, new ConstantTexture(Vec3(0, 0, 0)));
    return new HitableList(list, i);
}
*/

static Hitable *final_scene(float time0, float time1) {
    int nb = 20;

    Material *white = new Lambertian(new ConstantTexture(Vec3(0.73, 0.73, 0.73)));
    Material *ground = new Lambertian(new ConstantTexture(Vec3(0.48, 0.83, 0.53)));

    // Floor boxes.
    Hitable **box_list = new Hitable*[nb*nb];
    int b = 0;
    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            float width = 100;
            float x0 = -1000 + i*width;
            float y0 = 0;
            float z0 = -1000 + j*width;
            float x1 = x0 + width;
            float y1 = 100*(my_rand() + 0.01);
            float z1 = z0 + width;
            box_list[b++] = new Box(Vec3(x0, y0, z0), Vec3(x1, y1, z1), ground);
        }
    }

    Material *light = new DiffuseLight(new ConstantTexture(Vec3(7, 7, 7)));

    Hitable **list = new Hitable*[30];
    int l = 0;
    list[l++] = new Bvh(box_list, b, time0, time1);
    list[l++] = new XzRect(123, 423, 147, 412, 554, light);

    Vec3 center(400, 400, 200);
    list[l++] = new MovingSphere(center, center + Vec3(0, 30, 0), 0, 1, 50,
            new Lambertian(new ConstantTexture(Vec3(0.7, 0.3, 0.1))));
    list[l++] = new Sphere(Vec3(260, 150, 45), 50, new Dielectric(REF_GLASS));
    list[l++] = new Sphere(Vec3(0, 150, 145), 50, new Metal(Vec3(0.8, 0.8, 0.9), 1.0));
    Hitable *boundary = new Sphere(Vec3(360, 150, 145), 70, new Dielectric(REF_GLASS));
    list[l++] = boundary;
    list[l++] = new ConstantMedium(boundary, 0.2, new ConstantTexture(Vec3(0.2, 0.4, 0.9)));
    boundary = new Sphere(Vec3(0, 0, 0), 5000, new Dielectric(REF_GLASS));
    list[l++] = new ConstantMedium(boundary, 0.0001, new ConstantTexture(Vec3(1, 1, 1)));
    list[l++] = new Sphere(Vec3(400, 200, 400), 100,
            new Lambertian(new ImageTexture("data/earthmap.jpg")));
    list[l++] = new Sphere(Vec3(220, 280, 300), 80, new Lambertian(new NoiseTexture(0.1)));

    // Random spheres packed into a box shape.
    int sphere_count = 1000;
    Hitable **sphere_list = new Hitable*[sphere_count];
    for (int j = 0; j < sphere_count; j++) {
        sphere_list[j] = new Sphere(Vec3(165*my_rand(), 165*my_rand(), 165*my_rand()), 10, white);
    }
    list[l++] = new Translate(new RotateY(new Bvh(sphere_list, sphere_count, time0, time1), 15),
            Vec3(-100, 270, 395));

    return new HitableList(list, l);
}

static Vec3 trace_ray(const Ray &r, Hitable *world, int depth) {
    HitRecord rec;
    Vec3 color;

    if (world->hit(r, 0.001, MAXFLOAT, rec)) {
        Ray scattered;
        Vec3 attenuation;

        // Start with whatever color the material is emitting.
        color = rec.material->emitted(rec.u, rec.v, rec.p);

        // And add any scattering.
        if (depth < 50 && rec.material->scatter(r, rec, attenuation, scattered)) {
            color += attenuation*trace_ray(scattered, world, depth + 1);
        }
    } else {
        // Black background.
        color = Vec3(0, 0, 0);
    }

    return color;
}

// Trace line "j".
static void trace_line(unsigned char *row, int j, const Camera &cam, Hitable *world) {
    for (int i = 0; i < WIDTH; i++) {
        // Oversample.
        Vec3 c(0, 0, 0);
        for (int s = 0; s < SAMPLE_COUNT; s++) {
            float u = (i + my_rand())/WIDTH;
            float v = (j + my_rand())/HEIGHT;

            Ray r = cam.get_ray(u, v);
            c += trace_ray(r, world, 0);
        }
        c /= SAMPLE_COUNT;

        // Gamma correct.
        c = Vec3(sqrt(c.r()), sqrt(c.g()), sqrt(c.b()));

        // Clamp.
        c = c.min(VEC3_ONES);

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
        const Camera &cam, Hitable *world, int seed) {

    // Initialize the seed for our thread.
    init_rand(seed);

    for (int j = start; j < HEIGHT; j += skip) {
        unsigned char *row = image + j*STRIDE;

        trace_line(row, HEIGHT - 1 - j, cam, world);
        /// std::cerr << j << "\n";
    }
}

int main() {
    Vec3 look_from = Vec3(478, 278, -600);
    Vec3 look_at = Vec3(278, 278, 0);
    float focus_distance = 10;
    float aperature = 0.0;
    float vfov = 40;
    float time0 = 0;
    float time1 = 1;

    Camera cam(look_from, look_at, Vec3(0, 1, 0), vfov, float(WIDTH)/HEIGHT,
            aperature, focus_distance, time0, time1);

    Hitable *world = final_scene(time0, time1);

    unsigned char *image = new unsigned char[BYTE_COUNT];

    // Generate the image on multiple threads.
    std::thread *thread[THREAD_COUNT];
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t] = new std::thread(trace_lines, image, t, THREAD_COUNT, cam, world, random());
    }
    for (int t = 0; t < THREAD_COUNT; t++) {
        thread[t]->join();
        delete thread[t];
        thread[t] = 0;
    }

    // Write image.
    std::cout << "P6 " << WIDTH << " " << HEIGHT << " 255\n";
    std::cout.write((char *) image, BYTE_COUNT);

    return 0;
}
