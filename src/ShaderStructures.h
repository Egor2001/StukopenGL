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
struct SFragment;

SFragment to_fragment(const SVertex& vertex);
SFragment interpolate(const SFragment& beg, const SFragment& end, 
                      float ratio)

struct alignas(alignof(SVectorExt)) SVertex
{
    SVectorExt point;
    SVector    normal;
    SColor3    color;
    float      tex_u;
    float      tex_v;
};

struct alignas(32) SFragment
{
    SFragment(const SVector& point_set, const SVector& color_set, 
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
            SColor3 color;
            float   tex_u;
            float   tex_v;
        }

        __m256 as_ymm;
    }
};

SFragment to_fragment(const SVertex& vertex)
{
    return SFragment(to_vector(vertex.point), vertex.color,
                     vertex.tex_u, vertex.tex_v);
}

SFragment interpolate(const SFragment& beg, const SFragment& end, 
                      float ratio)
{
    return SFragment(_mm256_fnmadd_ps(_mm256_set1_ps(ratio),
                                      _mm256_add_ps(beg.as_ymm, end.as_ymm),
                                      beg.as_ymm);
}

//} //namespace sgl

#endif //SGL_SHADER_STRUCTURES_H

