#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include "Texture.h"

// A texture with a constant color.
class ConstantTexture : public Texture {
public:
    Vec3 m_color;

    ConstantTexture(const Vec3 &color)
        : m_color(color) {

        // Nothing.
    }

    virtual Vec3 value(float, float, const Vec3 &) const {
        return m_color;
    }
};

#endif // CONSTANT_TEXTURE_H
