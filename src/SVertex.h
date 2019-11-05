#ifndef SGL_SVERTEX_H
#define SGL_SVERTEX_H

//intrinsics (vectorization)
#include <immintrin.h>

//vertex
#include "math/SVectorExt.h"
#include "math/SColorExt.h"

//namespace sgl {

struct SVertex;

struct alignas(alignof(SVectorExt)) SVertex
{
    SVectorExt point;
    SVector    normal;
    SColor     color;
    float      tex_u;
    float      tex_v;
};

//} //namespace sgl

#endif //SGL_SVERTEX_H

