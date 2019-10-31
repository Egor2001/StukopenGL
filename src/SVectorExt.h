#ifndef SGL_SVECTOREXT_H
#define SGL_SVECTOREXT_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//intrinsics (vectorization)
#include <immintrin.h>

//vector
#include <cmath>
#include <cfloat>
#include "SVector.h"

//namespace sgl {

struct SVectorExt;

SVectorExt operator + (const SVectorExt& right, const SVectorExt& left);
SVectorExt operator - (const SVectorExt& right, const SVectorExt& left);
SVectorExt operator * (const SVectorExt& vector, float scale); 
    
float      dot(const SVectorExt& right, const SVectorExt& left);
SVectorExt mul(const SVectorExt& right, const SVectorExt& left);

SVector    narrow(const SVectorExt& extended);
SVectorExt extend(const SVector&    narrowed); 

struct alignas(16) SVectorExt
{
public:
    SVectorExt();
    SVectorExt(float x_set, float y_set, float z_set, float w_set = 1.0f);
    SVectorExt(const float vec_set[4]);

          float& operator [] (size_t idx)       noexcept;
    const float& operator [] (size_t idx) const noexcept;
       
    SVectorExt operator - () const;

    SVectorExt& operator += (const SVectorExt& add);
    SVectorExt& operator -= (const SVectorExt& sub);
    SVectorExt& operator *= (float scale);

public:
    union
    {
        struct { float x, y, z, w; };
        float vec[4];
        __m128 as_xmm;
    };
};

SVectorExt::SVectorExt():
    as_xmm(_mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f)) 
{}

SVectorExt::SVectorExt(float x_set, float y_set, float z_set, float w_set):
    as_xmm(_mm_setr_ps(x_set, y_set, z_set, w_set)) 
{}

SVectorExt::SVectorExt(const float vec_set[4]):
    as_xmm(_mm_loadu_ps(vec_set)) 
{}

float& SVectorExt::operator [] (size_t idx) noexcept
{
    return vec[idx];
}

const float& SVectorExt::operator [] (size_t idx) const noexcept
{
    return vec[idx];
}

SVectorExt SVectorExt::operator - () const
{
    return SVectorExt(-x, -y, -z, w);
}

SVectorExt& SVectorExt::operator += (const SVectorExt& add)
{
    float new_w = w*add.w;

    //exists only on Intel Haswell and newer
    if (false)
    {
//        as_xmm = _mm_fmadd_ps(_mm_set1_ps(add.w), as_xmm, 
//                              _mm_mul_ps(_mm_set1_ps(w), add.as_xmm));
    }
    else
    {
        as_xmm = _mm_add_ps(_mm_mul_ps(as_xmm, _mm_set1_ps(add.w)),
                            _mm_mul_ps(add.as_xmm, _mm_set1_ps(w)));
    }

    w = new_w;

    return *this;
}

SVectorExt& SVectorExt::operator -= (const SVectorExt& sub)
{
    float new_w = w*sub.w;

    //exists only on Intel Haswell and newer
    if (false)
    {
//        as_xmm = _mm_fmsub_ps(_mm_set1_ps(sub.w), as_xmm, 
//                              _mm_mul_ps(_mm_set1_ps(w), sub.as_xmm));
    }
    else
    {
        as_xmm = _mm_sub_ps(_mm_mul_ps(as_xmm, _mm_set1_ps(sub.w)),
                            _mm_mul_ps(sub.as_xmm, _mm_set1_ps(w)));
    }

    w = new_w;

    return *this;
}

SVectorExt& SVectorExt::operator *= (float scale)
{
    float new_w = w;
    as_xmm = _mm_mul_ps(as_xmm, _mm_set1_ps(scale));

    w = new_w;

    return *this;
}

SVectorExt operator + (const SVectorExt& right, const SVectorExt& left)
{
    SVectorExt result = right;
    return (result += left);    
}

SVectorExt operator - (const SVectorExt& right, const SVectorExt& left)
{
    SVectorExt result = right;
    return (result -= left);    
}

SVectorExt operator * (const SVectorExt& vector, float scale) 
{
    SVectorExt result = vector;
    return (result *= scale);    
}

float dot(const SVectorExt& right, const SVectorExt& left)
{
    float result = _mm_cvtss_f32(_mm_dp_ps(right.as_xmm, left.as_xmm, 
                                           0b01111111)/(right.w*left.w)); 

    return result;
}
    
SVectorExt mul(const SVectorExt& right, const SVectorExt& left)
{
    SVectorExt result;

    result.x = right.y*left.z - right.z*left.y;
    result.y = right.z*left.x - right.x*left.z;
    result.z = right.x*left.y - right.y*left.x;
    result.w = right.w*left.w;

    return result;
}

//could return inf's and nan's 
SVector narrow(const SVectorExt& extended)
{
    float scale = 1.0f/extended.w;

    SVector result = SVector{ extended.x, 
                              extended.y, 
                              extended.z }*scale;

    return result;
}

SVectorExt extend(const SVector& narrowed) 
{
    SVectorExt result = SVectorExt(narrowed.x, 
                                   narrowed.y, 
                                   narrowed.z, 
                                   1.0f); 

    return result;
}

int test_SVectorExt()
{
    SVectorExt vec1 = SVectorExt(0.5f, 6.7f, -3.4f);
    SVectorExt vec2 = SVectorExt(-4.3f, 0.1f, 2.6f, 0.5f);

    SVectorExt add = vec1 + vec2;
    SVectorExt sub = vec1 - vec2;
    SVectorExt mul_vec = mul(vec1, vec2);
    float      mul_dot = dot(vec1, vec2);

    printf("vec1    %f %f %f %f\n", vec1.x, vec1.y, vec1.z, vec1.w);
    printf("vec2    %f %f %f %f\n", vec2.x, vec2.y, vec2.z, vec2.w);
    printf("add     %f %f %f %f\n", add.x, add.y, add.z, add.w);
    printf("sub     %f %f %f %f\n", sub.x, sub.y, sub.z, sub.w);
    printf("mul_vec %f %f %f %f\n", mul_vec.x, mul_vec.y, mul_vec.z, mul_vec.w);
    printf("mul_dot %f\n", mul_dot);

    return 0;
}

//} //namespace sgl

#endif //EGL_SVECTOREXT_H
