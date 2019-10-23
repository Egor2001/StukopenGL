#ifndef SGL_CVERTEXSHADER_H
#define SGL_CVERTEXSHADER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions
#include "SVector.h"
#include "SMatrix.h"
#include "SColor.h" 

//namespace sgl {

class CVertexShader
{
public:
    using in_t  = SVertex;
    using out_t = SVertex;

    CVertexShader();
    explicit CVertexShader(const SMatrix& mtx_set);

    CVertexShader             (const CVertexShader&);
    CVertexShader& operator = (const CVertexShader&);

    CVertexShader             (CVertexShader&&);
    CVertexShader& operator = (CVertexShader&&);

    ~CVertexShader();

    out_t apply(const in_t& in) const;
    
private:
    SMatrix mtx_; 
};

CVertexShader::CVertexShader():
    mtx_()
{}

CVertexShader::CVertexShader(const SMatrix& mtx_set):
    mtx_(mtx_set)
{}

CVertexShader::CVertexShader(const CVertexShader& copy_shader):
    mtx_(copy_shader.mtx_)
{}

CVertexShader& CVertexShader::operator = (const CVertexShader& copy_shader)
{
    mtx_ = copy_shader.mtx_;

    return *this;
}

CVertexShader::CVertexShader(CVertexShader&& move_shader):
    mtx_(std::move(move_shader.mtx_))
{}

CVertexShader& CVertexShader::operator = (CVertexShader&& move_shader)
{
    std::swap(mtx_, move_shader.mtx_);

    return *this;
}

CVertexShader::~CVertexShader()
{
    mtx_ = SMatrix();
}

CVertexShader::out_t CVertexShader::apply(const in_t& in) const
{
    out_t result = out_t();

    result.point  = unitary(mtx_*in.point); 
    result.normal = normal (mtx_*in.normal); 
    result.color  =              in.color;
 
    return result;
}
 
int test_CVertexShader()
{
    return 0;
}

//} //namespace sgl

#endif //SGL_CVERTEXSHADER_H
