#ifndef SGL_SVERTEXSHADER_H
#define SGL_SVERTEXSHADER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions
#include "ShaderStructures.h"
#include "SVectorExt.h"
#include "SMatrixExt.h"
#include "SCamera.h" 
#include "SLight.h" 

//namespace sgl {

struct SVertexShader
{
public:
    using in_t  = SVertex;
    using out_t = SVertex;

    out_t apply(const in_t&) const;
    out_t apply(const in_t&, const SLight&) const;
    out_t apply(const in_t&, const SLight&, const SVector&) const;
    
    SMatrixExt modelview_mtx; 
    SMatrixExt projection_mtx; 
};

typename SVertexShader::out_t 
SVertexShader::apply(const in_t& in) const
{
    out_t result = out_t();

    result.point  = projection_mtx*modelview_mtx*in.point; 
    result.normal = ::normal(modelview_mtx*in.normal); 
    result.color  = in.color;
 
    return result;
}

typename SVertexShader::out_t 
SVertexShader::apply(const in_t& in, 
                     const SLight& light) const
{
    out_t result = out_t();

    result.point  = modelview_mtx*in.point; 
    result.normal = ::normal(modelview_mtx*in.normal); 
    result.color  = in.color;
    result.color  = light.apply(result);
    result.point  = projection_mtx*result.point; 
 
    return result;
}

typename SVertexShader::out_t 
SVertexShader::apply(const in_t& in, 
                     const SLight& light, 
                     const SVector& eyepos) const
{
    out_t result = out_t();

    result.point  = modelview_mtx*in.point; 
    result.normal = ::normal(modelview_mtx*in.normal); 
    result.color  = in.color;
    result.color  = light.apply(result, eyepos);
    result.point  = projection_mtx*result.point; 
 
    return result;
}
 
int test_SVertexShader()
{
    return 0;
}

//} //namespace sgl

#endif //SGL_SVERTEXSHADER_H
