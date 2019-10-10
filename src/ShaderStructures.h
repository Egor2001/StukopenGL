#ifndef SGL_SHADER_STRUCTURES_H
#define SGL_SHADER_STRUCTURES_H

//general
#include <cstdint>
#include <cfloat>

//vertex
#include "SVector.h"
#include "SColor.h"

//namespace sgl {

struct SFragment
{
    struct SFragColor
    {
        uint8_t r, g, b, a;
    };

    size_t x, y;
    float      depth;
    SFragColor color;
};

struct SVertex
{
    SVector point;
    SVector normal;
    SColor  color;
};

//} //namespace sgl

#endif //SGL_SHADER_STRUCTURES_H

