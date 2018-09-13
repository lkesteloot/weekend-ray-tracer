#ifndef CHECKER_TEXTURE_H
#define CHECKER_TEXTURE_H

#include "Texture.h"

// A texture with a checker pattern.
class CheckerTexture : public Texture {
public:
    Texture *m_odd;
    Texture *m_even;
    float m_omega;

    // Size is the size of one checker block.
    CheckerTexture(Texture *odd, Texture *even, float size)
        : m_odd(odd), m_even(even), m_omega(M_PI/size) {

        // Nothing.
    }

    virtual Vec3 value(float u, float v, const Vec3 &p) const {
        float sines = sin(m_omega*p.x())*sin(m_omega*p.y())*sin(m_omega*p.z());

        Texture *texture = sines < 0 ? m_odd : m_even;

        return texture->value(u, v, p);
    }
};

#endif // CHECKER_TEXTURE_H
