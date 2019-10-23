#ifndef SGL_SCOLOR_H
#define SGL_SCOLOR_H

//color
#include <cfloat>

//namespace sgl {

struct SColor;

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio);
SColor alpha_blend(const SColor& beg_c, const SColor& end_c);
SColor operator * (const SColor& color, float scale);

struct SColor
{
    SColor():
        r(0.0f), g(0.0f), b(0.0f), a(1.0f)
    {} 

    //TODO: use clamp
    SColor(float r_set, float g_set, float b_set, float a_set = 1.0f):
        r(fmax(0.0f, fmin(r_set, 1.0f))), 
        g(fmax(0.0f, fmin(g_set, 1.0f))), 
        b(fmax(0.0f, fmin(b_set, 1.0f))), 
        a(fmax(0.0f, fmin(a_set, 1.0f)))
    {}

    float r, g, b, a;
};

SColor interpolate(const SColor& beg_c, const SColor& end_c, float ratio)
{
    SColor result = SColor();

    result.r = beg_c.r*(1.0f - ratio) + end_c.r*ratio;
    result.g = beg_c.g*(1.0f - ratio) + end_c.g*ratio;
    result.b = beg_c.b*(1.0f - ratio) + end_c.b*ratio;
    result.a = beg_c.a*(1.0f - ratio) + end_c.a*ratio;

    return result;
}

SColor alpha_blend(const SColor& beg_c, const SColor& end_c)
{
    SColor result = SColor();

    float ratio = 0.0f;

    float sum_alpha = beg_c.a + end_c.a;
    if (sum_alpha < FLT_EPSILON)
        ratio = 0.5f;
    else
        ratio = (end_c.a / sum_alpha);  
    
    result = interpolate(beg_c, end_c, ratio);

    return result;
}

SColor operator * (const SColor& color, float scale)
{
    SColor result = color;

    result = interpolate(SColor(), color, scale); 

    return result;
}

//} //namespace sgl 

#endif //SGL_SCOLOR_H
