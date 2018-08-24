#include <iostream>
#include "Vec3.h"

int main() {
    int nx = 200;
    int ny = 100;

    std::cout << "P3 " << nx << " " << ny << " 255\n";
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = nx - 1; i >= 0; i--) {
            Vec3 color(float(i)/nx, float(j)/ny, 0.2f);
            int ir = int(255.99*color.r());
            int ig = int(255.99*color.g());
            int ib = int(255.99*color.b());

            std::cout << ir << " " << ig << " " << ib << "\n";
        }
    }

    return 0;
}
