#ifndef SGL_CLIGHT_H
#define SGL_CLIGHT_H

#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "SVectorExt.h"
#include "SColor.h"

//namespace sgl {

class CLight
{
public:
    explicit CLight(const SVectorExt& pos_set, 
                    const SColor& = SColor{ 1.0f, 1.0f, 1.0f });

    CLight             (const CLight&&);
    CLight& operator = (const CLight&&);
    CLight             (CLight&&);
    CLight& operator = (CLight&&);

    ~CLight();

    SVertex apply(const SVertex& vertex) const;

private:
    SVectorExt pos_;
    SColor  color_;
};

CLight::CLight(const SVectorExt& pos_set, const SColor& color_set):
    pos_  (pos_set),
    color_(color_set)
{}

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
    color_ = SColor{ 0.0f, 0.0f, 0.0f };
    pos_   = SVectorExt();
}
    
SVertex CLight::apply(const SVertex& vertex) const
{
    SVertex result = vertex;

    SVector dir_to = ::normal(narrow(vertex.point - pos_));

    if (fabs(pos_.w) < FLT_EPSILON)
        dir_to = ::normal(-SVector{ pos_.x, pos_.y, pos_.z });
    else
        dir_to = ::normal(narrow(vertex.point - pos_)); 

    SVector normal = vertex.normal;

    float scale = fmax(0.0f, dot(dir_to, normal));

    result.color = vertex.color * scale; 

    return result;
}

//} //namespace sgl

#endif //SGL_CLIGHT_H
