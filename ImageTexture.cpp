
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ImageTexture.h"

unsigned char *load_image(char const *pathname, int &width, int &height) {
    int components;

    unsigned char *image = stbi_load(pathname, &width, &height, &components, 0);
    if (components != 3) {
        std::cerr << "Can't handle images with " << components << " components.\n";
        return 0;
    }

    return image;
}
