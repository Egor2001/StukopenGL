#ifndef SGL_SHADER_STRUCTURES_H
#define SGL_SHADER_STRUCTURES_H

//general
#include <cstdint>
#include <cfloat>

//intrinsics (vectorization)
#include <immintrin.h>

//vertex
#include "SVector.h"
#include "SColor.h"

//namespace sgl {

struct SVertex;
struct SFragmentExt;

SFragmentExt to_fragment(const SVertex& vertex);
SFragmentExt interpolate(const SFragmentExt& beg, const SFragmentExt& end, 
                         float ratio);

struct alignas(alignof(SVectorExt)) SVertex
{
    SVectorExt point;
    SVector    normal;
    SColor     color;
    float      tex_u;
    float      tex_v;
};

struct SFragment
{
    SVector point; 
    SColor  color;
    float   tex_u;
    float   tex_v;
};

struct alignas(32) SFragmentExt
{
    SFragmentExt():
        as_ymm(_mm256_set1_ps(0.0f))
    {}

    SFragmentExt(const SVector& point_set, const SColor& color_set, 
                 float tex_u_set = 0.0f, float tex_v_set = 0.0f):
        frag{ .point=point_set, .color=color_set, 
              .tex_u=tex_u_set, .tex_v=tex_v_set }
    {}

    explicit SFragmentExt(__m256 as_ymm_set):
        as_ymm(as_ymm_set) 
    {}

    union 
    {
        SFragment frag;
        __m256 as_ymm;
    };
};

SFragmentExt to_fragment(const SVertex& vertex)
{
    return SFragmentExt(narrow(vertex.point), vertex.color,
                        vertex.tex_u, vertex.tex_v);
}

SFragmentExt interpolate(const SFragmentExt& beg, const SFragmentExt& end, 
                         float ratio)
{
    //exists only on Intel Haswell and newer
    if (false)
    {
//        return SFragmentExt(_mm256_fnmadd_ps(_mm256_set1_ps(ratio),
//                                          _mm256_add_ps(beg.as_ymm, 
//                                                        end.as_ymm),
//                                          beg.as_ymm);
    }
    else
    {
        return SFragmentExt(_mm256_add_ps(beg.as_ymm, 
                                       _mm256_mul_ps(_mm256_sub_ps(end.as_ymm,
                                                                   beg.as_ymm),
                                                     _mm256_set1_ps(ratio))));
    }
}

//} //namespace sgl

#endif //SGL_SHADER_STRUCTURES_H

