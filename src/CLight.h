#ifndef SGL_CLIGHT_H
#define SGL_CLIGHT_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "SVector.h"
#include "SColor.h"

//namespace sgl {

class CLight
{
public:
    explicit CLight(const SVector& pos_set, 
                    const SColor& = SColor(1.0f, 1.0f, 1.0f));

    CLight             (const CLight&&);
    CLight& operator = (const CLight&&);
    CLight             (CLight&&);
    CLight& operator = (CLight&&);

    ~CLight();

    SVertex apply(const SVertex& vertex) const;

private:
    SVector pos_;
    SColor  color_;
};

CLight::CLight(const SVector& pos_set, const SColor& color_set):
    pos_  (pos_set),
    color_(color_set)
{
    if (fabs(pos_.w) > FLT_EPSILON)
        pos_ = unitary(pos_);
    else
        pos_ = normal(pos_);
}

CLight::CLight(const CLight&& copy_light):
    pos_  (copy_light.pos_),
    color_(copy_light.color_)
{}

CLight& CLight::operator = (const CLight&& copy_light)
{
    pos_   = copy_light.pos_;
    color_ = copy_light.color_;

    return *this;
}

CLight::CLight(CLight&& move_light):
    pos_  (std::move(move_light.pos_)),
    color_(std::move(move_light.color_))
{}

CLight& CLight::operator = (CLight&& move_light)
{
    pos_   = std::move(move_light.pos_);
    color_ = std::move(move_light.color_);

    return *this;
}

CLight::~CLight()
{
    color_ = SColor(0.0f, 0.0f, 0.0f);
    pos_   = SVector();
}
    
SVertex CLight::apply(const SVertex& vertex) const
{
    SVertex result = vertex;

    SVector dir_to = SVector();
    SVector normal = vertex.normal;

    if (fabs(pos_.w) > FLT_EPSILON) dir_to = ::normal(vertex.point - pos_);
    else                            dir_to = pos_;

    float scale = fmax(0.0f, scalar_mul(dir_to, normal));

    result.color = vertex.color * scale; 

    return result;
}

//} //namespace sgl

#endif //SGL_CLIGHT_H
