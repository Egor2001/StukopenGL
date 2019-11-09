#ifndef SGL_SCOLOR_H
#define SGL_SCOLOR_H

//color
#include <cfloat>

//namespace sgl {

struct SColor;

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio);

SColor& operator *= (SColor& color, float ratio);
SColor& operator *= (SColor& color, const SColor& mul);

SColor operator * (const SColor& color, float ratio);
SColor operator * (const SColor& lhs, const SColor& rhs);

struct SColor
{
    float r, g, b;
};

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio)
{
    SColor result = SColor{}; 

    result.r = beg_c.r*(1.0f - ratio) + end_c.r*ratio;
    result.g = beg_c.g*(1.0f - ratio) + end_c.g*ratio;
    result.b = beg_c.b*(1.0f - ratio) + end_c.b*ratio;

    return result;
}

SColor& operator *= (SColor& color, float ratio)
{
    color.r *= ratio;
    color.g *= ratio;
    color.b *= ratio;

    return color;
}

SColor& operator *= (SColor& color, const SColor& mul)
{
    color.r *= mul.r;
    color.g *= mul.g;
    color.b *= mul.b;

    return color;
}

SColor operator * (const SColor& color, float ratio)
{
    return interpolate(SColor{}, color, ratio);
}

SColor operator * (const SColor& lhs, const SColor& rhs)
{
    return SColor{ lhs.r*rhs.r, lhs.g*rhs.g, lhs.b*rhs.b };
}

//} //namespace sgl 

#endif //SGL_SCOLOR_H
