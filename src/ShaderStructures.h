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
    struct SVertColor
    {
        float r, g, b;
    };

    SVector point;
    SVector normal;
    SColor  color;
};

SVertex interpolate(const SVertex& beg_v, const SVertex& end_v,
                    float ratio)
{
    SVertex result = {};

    result.point = (beg_v.point * (1.0f - ratio) +
                    end_v.point *         ratio);

//    result.normal = (beg_v.normal * (1.0f - ratio) +
//                     end_v.normal *         ratio);

    result.color = ::interpolate(beg_v.color, end_v.color, ratio);

    return result;
}

//} //namespace sgl

#endif //SGL_SHADER_STRUCTURES_H

