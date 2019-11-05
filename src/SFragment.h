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

//} //namespace sgl

#endif //SGL_SFRAGMENT_H

