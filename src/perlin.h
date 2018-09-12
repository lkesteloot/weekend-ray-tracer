#ifndef PERLIN_H
#define PERLIN_H

#include "Vec3.h"

// Must be power of two.
#define GRID_SIZE 256
#define GRID_MASK (GRID_SIZE - 1)

static Vec3 *generate_vecs();
static int *generate_permutation();

// GRID_SIZE random float numbers.
static Vec3 *g_random_vec = generate_vecs();

// GRID_SIZE integers permuting numbers [0, GRID_SIZE).
static int *g_permute_x = generate_permutation();
static int *g_permute_y = generate_permutation();
static int *g_permute_z = generate_permutation();

static Vec3 *generate_vecs() {
    Vec3 *v = new Vec3[GRID_SIZE];

    for (int i = 0; i < GRID_SIZE; i++) {
        v[i] = Vec3(
                -1 + 2*my_rand(),
                -1 + 2*my_rand(),
                -1 + 2*my_rand()).unit();
    }

    return v;
}

static int *generate_permutation() {
    int *perm = new int[GRID_SIZE];

    // Start with an ordered array.
    for (int i = 0; i < GRID_SIZE; i++) {
        perm[i] = i;
    }

    // Then permute it.
    for (int i = GRID_SIZE - 1; i > 0; i--) {
        int j = int(my_rand()*(i + 1));

        std::swap(perm[i], perm[j]);
    }

    return perm;
}

inline float trilinear_interp(Vec3 c[2][2][2], float u, float v, float w) {
    float a = 0;

    // Hermite cubic.
    float uu = u*u*(3 - 2*u);
    float vv = v*v*(3 - 2*v);
    float ww = w*w*(3 - 2*w);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                Vec3 weight(u - i, v - j, w - k);

                a += (i*uu + (1 - i)*(1 - uu))*
                    (j*vv + (1 - j)*(1 - vv))*
                    (k*ww + (1 - k)*(1 - ww))*
                    c[i][j][k].dot(weight);
            }
        }
    }

    return a;
}

// Returns a number -1 to 1.
float perlin_noise(const Vec3 &p) {
    // Integer parts.
    int i = floor(p.x());
    int j = floor(p.y());
    int k = floor(p.z());

    // Fractional parts.
    float u = p.x() - i;
    float v = p.y() - j;
    float w = p.z() - k;

    // Find eight values around this point.
    Vec3 c[2][2][2];

    for (int di = 0; di < 2; di++) {
        for (int dj = 0; dj < 2; dj++) {
            for (int dk = 0; dk < 2; dk++) {
                // Were in the 3D grid we are.
                int ci = g_permute_x[(i + di) & GRID_MASK];
                int cj = g_permute_y[(j + dj) & GRID_MASK];
                int ck = g_permute_z[(k + dk) & GRID_MASK];

                c[di][dj][dk] = g_random_vec[ci ^ cj ^ ck];
            }
        }
    }

    return trilinear_interp(c, u, v, w);
}

// Returns a number 0 to 1.
float perlin_turb(const Vec3 &p, int depth) {
    float a = 0;
    Vec3 temp_p = p;
    float weight = 1;

    for (int i = 0; i < depth; i++) {
        a += weight*perlin_noise(temp_p);
        weight *= 0.5;
        temp_p *= 2;
    }

    return fabs(a);
}

#endif // PERLIN_H
