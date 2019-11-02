#ifndef SGL_SLIGHT_H
#define SGL_SLIGHT_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "SVectorExt.h"
#include "SColor.h"

//namespace sgl {

struct SLight
{
public:
//no constructors to make type trivial
    SColor apply(const SVertex& vertex) const;
    SColor apply(const SVertex& vertex, const SVector& eyepos) const;

    SVector point;
    SColor  color;
    SVector phong_ads;
    float   phong_pow;
};

SColor SLight::apply(const SVertex& vertex) const
{
    SColor result;

    SVector vecpos = ::narrow(vertex.point);
    SVector dir_to = ::normal(point - vecpos);

    float ambient  = 1.0f;
    float diffuse  = fmax(0.0f, dot(dir_to, vertex.normal));
    float specular = 0.0f; 

    float scale = dot(phong_ads, SVector{ ambient, diffuse, specular });
    result = (vertex.color*color)*scale; 

    return result;
}

SColor SLight::apply(const SVertex& vertex, const SVector& eyepos) const
{
    SColor result;

    SVector vecpos = ::narrow(vertex.point);
    SVector dir_to = ::normal(point - vecpos);
    SVector medial = ::normal(dir_to + ::normal(eyepos - vecpos));

    float ambient = 1.0f;
    float diffuse = fmax(0.0f, -dot(dir_to, vertex.normal));
    float specular = powf(fmax(0.0f, -dot(medial, vertex.normal)), phong_pow); 

    float scale = dot(phong_ads, SVector{ ambient, diffuse, specular });
    result = (vertex.color*color)*scale; 

    return result;
}

//} //namespace sgl

#endif //SGL_SLIGHT_H
