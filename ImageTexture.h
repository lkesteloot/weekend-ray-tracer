#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "Texture.h"

// Returns RGB-packed image.
unsigned char *load_image(char const *pathname, int &width, int &height);

// A texture with an image.
class ImageTexture : public Texture {
public:
    unsigned char *m_pixels;
    int m_width;
    int m_height;

    // Pixels should be packed RGB.
    ImageTexture(unsigned char *pixels, int width, int height)
        : m_pixels(pixels), m_width(width), m_height(height) {

        // Nothing.
    }

    ImageTexture(char const *pathname) {
        m_pixels = load_image(pathname, m_width, m_height);
    }

    virtual Vec3 value(float u, float v, const Vec3 &) const {
        // Consider lower-left of image to be its origin.
        int i = u*m_width;
        int j = m_height - 1 - v*m_height;

        // Clamp to image.
        i = std::min(std::max(i, 0), m_width - 1);
        j = std::min(std::max(j, 0), m_height - 1);

        int index = 3*(i + m_width*j);
        float r = m_pixels[index] / 255.0;
        float g = m_pixels[index + 1] / 255.0;
        float b = m_pixels[index + 2] / 255.0;

        return Vec3(r, g, b);
    }
};

#endif // IMAGE_TEXTURE_H
