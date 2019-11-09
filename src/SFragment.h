#ifndef SGL_SFRAGMENT_H
#define SGL_SFRAGMENT_H

//intrinsics (vectorization)
#include <immintrin.h>

//vertex
#include "math/SVectorExt.h"
#include "math/SColorExt.h"
#include "SVertex.h"

//namespace sgl {

struct SFragment;

SFragment to_fragment(const SVertex& vertex);
SFragment interpolate(const SFragment& beg, const SFragment& end, 
                      float ratio);

void clamp_x(SFragment& beg, SFragment& end, float min_x, float max_x);
void clamp_y(SFragment& beg, SFragment& end, float min_y, float max_y);

struct alignas(32) SFragment
{
    SFragment():
        as_ymm(_mm256_set1_ps(0.0f))
    {}

    SFragment(const SVector& point_set, const SColor& color_set, 
              float tex_u_set = 0.0f, float tex_v_set = 0.0f):
        point(point_set), color(color_set), 
        tex_u(tex_u_set), tex_v(tex_v_set)
    {}

    explicit SFragment(__m256 as_ymm_set):
        as_ymm(as_ymm_set) 
    {}

    union 
    {
        struct
        {
            SVector point; 
            SColor  color;
            float   tex_u;
            float   tex_v;
        };

        __m256 as_ymm;
    };
};

SFragment to_fragment(const SVertex& vertex)
{
    return SFragment(narrow(vertex.point), 
                     vertex.color,
                     vertex.tex_u, vertex.tex_v);
}

SFragment interpolate(const SFragment& beg, const SFragment& end, 
                      float ratio)
{
    //exists only on Intel Haswell and newer
    if (false)
    {
//        return SFragment(_mm256_fnmadd_ps(_mm256_set1_ps(ratio),
//                                             _mm256_add_ps(beg.as_ymm, 
//                                                           end.as_ymm),
//                                             beg.as_ymm);
    }
    else
    {
        return SFragment(_mm256_add_ps(beg.as_ymm, 
                                       _mm256_mul_ps(_mm256_sub_ps(end.as_ymm,
                                                                   beg.as_ymm),
                                                     _mm256_set1_ps(ratio))));
    }
}

void clamp_x(SFragment& beg, SFragment& end,
             float min_x, float max_x)
{
    if (beg.point.x < min_x && end.point.x < min_x) 
        beg.point.x = end.point.x = min_x;

    if (beg.point.x > max_x && end.point.x > max_x)
        beg.point.x = end.point.x = max_x;

    if (fabs(end.point.x - beg.point.x) > FLT_EPSILON)
    {
        float x_scale = 1.0f/(end.point.x - beg.point.x);

        beg = interpolate(end, beg, 
                (end.point.x - fmax(0.0f, fmin(beg.point.x, max_x)))*x_scale);
        
        end = interpolate(beg, end, 
                (fmax(0.0f, fmin(end.point.x, max_x)) - beg.point.x)*x_scale);
    }
}

void clamp_y(SFragment& beg, SFragment& end,
             float min_y, float max_y)
{
    if (beg.point.y < min_y && end.point.y < min_y) 
        beg.point.y = end.point.y = min_y;

    if (beg.point.y > max_y && end.point.y > max_y)
        beg.point.y = end.point.y = max_y;

    if (fabs(end.point.y - beg.point.y) > FLT_EPSILON)
    {
        float x_scale = 1.0f/(end.point.y - beg.point.y);

        beg = interpolate(end, beg, 
                (end.point.y - fmax(0.0f, fmin(beg.point.y, max_y)))*x_scale);
        
        end = interpolate(beg, end, 
                (fmax(0.0f, fmin(end.point.y, max_y)) - beg.point.y)*x_scale);
    }
}

//} //namespace sgl

#endif //SGL_SFRAGMENT_H

