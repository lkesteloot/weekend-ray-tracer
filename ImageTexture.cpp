
#include "stb_image.h"

#include "ImageTexture.h"

ImageTexture::ImageTexture(char const *pathname) {
    m_byte_pixels = 0;
    m_float_pixels = 0;

    if (stbi_is_hdr(pathname)) {
        int components;

        m_float_pixels = stbi_loadf(pathname, &m_width, &m_height, &components, 0);
        if (components != 3) {
            std::cerr << "Can't handle images with " << components << " components.\n";
        }
    } else {
        m_byte_pixels = load_image(pathname, m_width, m_height);
    }
}

unsigned char *load_image(char const *pathname, int &width, int &height) {
    int components;

    unsigned char *image = stbi_load(pathname, &width, &height, &components, 0);
    if (components != 3) {
        std::cerr << "Can't handle images with " << components << " components.\n";
        return 0;
    }

    return image;
}
