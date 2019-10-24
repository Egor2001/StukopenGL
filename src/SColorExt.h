#ifndef SGL_SCOLOREXT_H
#define SGL_SCOLOREXT_H

#include <immintrin.h>

//color
#include <cfloat>

//namespace sgl {

struct SColorExt;

SColorExt interpolate(const SColorExt& beg_c, const SColorExt& end_c, 
                      float ratio);
SColorExt alpha_blend(const SColorExt& beg_c, const SColorExt& end_c);
SColorExt operator * (const SColorExt& color, float scale);

struct SColorExt
{
    SColorExt():
        r(0.0f), g(0.0f), b(0.0f), a(1.0f)
    {} 

    //TODO: use clamp
    SColorExt(float r_set, float g_set, float b_set, float a_set = 1.0f):
        r(fmax(0.0f, fmin(r_set, 1.0f))), 
        g(fmax(0.0f, fmin(g_set, 1.0f))), 
        b(fmax(0.0f, fmin(b_set, 1.0f))), 
        a(fmax(0.0f, fmin(a_set, 1.0f)))
    {}

    union
    {
        struct { float r, g, b, a; };
        __m128 as_xmm;
    };
};

SColorExt interpolate(const SColorExt& beg_c, const SColorExt& end_c, 
                      float ratio)
{
    SColorExt result = SColorExt();

    result.as_xmm = _mm_add_ps(beg_c.as_xmm, 
                               _mm_mul_ps(_mm_sub_ps(end_c.as_xmm,
                                                     beg_c.as_xmm),
                                          _mm_set1_ps(ratio));

    return result;
}

SColorExt alpha_blend(const SColorExt& beg_c, const SColorExt& end_c)
{
    SColorExt result = SColorExt();

    float ratio = 0.0f;

    float sum_alpha = beg_c.a + end_c.a;
    if (sum_alpha < FLT_EPSILON)
        ratio = 0.5f;
    else
        ratio = (end_c.a / sum_alpha);  
    
    result = interpolate(beg_c, end_c, ratio);

    return result;
}

SColorExt operator * (const SColorExt& color, float scale)
{
    SColorExt result = color;

    result = interpolate(SColorExt(), color, scale); 

    return result;
}

//} //namespace sgl 

#endif //SGL_SCOLOREXT_H
