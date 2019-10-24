#ifndef SGL_SCOLOR_H
#define SGL_SCOLOR_H

//color
#include <cfloat>

//namespace sgl {

struct SColor;

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio);
SColor operator * (const SColor& color, float ratio);

struct SColor
{
    SColor():
        r(0.0f), g(0.0f), b(0.0f)
    {} 

    //TODO: use clamp
    SColor(float r_set, float g_set, float b_set):
        r(fmax(0.0f, fmin(r_set, 1.0f))), 
        g(fmax(0.0f, fmin(g_set, 1.0f))), 
        b(fmax(0.0f, fmin(b_set, 1.0f))) 
    {}

    float r, g, b;
};

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio)
{
    SColor result = SColor();

    result.r = beg_c.r*(1.0f - ratio) + end_c.r*ratio;
    result.g = beg_c.g*(1.0f - ratio) + end_c.g*ratio;
    result.b = beg_c.b*(1.0f - ratio) + end_c.b*ratio;

    return result;
}

SColor operator * (const SColor& color, float ratio)
{
    return interpolate(SColor(), color, ratio);
}

//} //namespace sgl 

#endif //SGL_SCOLOR_H
