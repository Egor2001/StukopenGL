#ifndef SGL_SVECTOR_H
#define SGL_SVECTOR_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//intrinsics (vectorization)
#include <xmmintrin.h>

//vector
#include <cmath>
#include <cfloat>

//namespace sgl {

struct SVector;
struct SVectorExt;

SVectorExt operator + (const SVectorExt& right, const SVectorExt& left);
SVectorExt operator - (const SVectorExt& right, const SVectorExt& left);
SVectorExt operator * (const SVectorExt& vector, float scale); 
    
SVectorExt vector_mul(const SVectorExt& right, const SVectorExt& left);
float      scalar_mul(const SVectorExt& right, const SVectorExt& left);

float      abs    (const SVectorExt& vector);
SVectorExt unitary(const SVectorExt& vector);
SVectorExt normal (const SVectorExt& vector);

struct SVector
{
public:
    SVector();
    SVector(float x_set, float y_set, float z_set);

    float  
        
    float x, y, z;
};

struct SVectorExt
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
        struct { float x, y, z, w; }
        float vec[4];
        __m128 as_xmm;
    }
};

SVectorExt::SVectorExt():
    as_xmm(_mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f)) 
{}

SVectorExt::SVectorExt(float x_set, float y_set, float z_set, float w_set):
    as_xmm(_mm_set_ps(x_set, y_set, z_set, w_set)) 
{}

SVectorExt::SVectorExt(const float vec_set[4]):
    as_xmm(_mm_load_ps(vec_set)) 
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
    as_xmm = _mm_madd_ps(_mm_set1_ps(add.w), as_xmm, 
                         _mm_mul_ps(_mm_set1_ps(w), add.as_xmm));

    w = new_w;

    return *this;
}

SVectorExt& SVectorExt::operator -= (const SVectorExt& sub)
{
    float new_w = w*sub.w;
    as_xmm = _mm_msub_ps(_mm_set1_ps(sub.w), as_xmm, 
                         _mm_mul_ps(_mm_set1_ps(w), sub.as_xmm));

    w = new_w;

    return *this;
}

SVectorExt& SVectorExt::operator *= (float scale)
{
    float new_w = w;
    as_xmm = _mm_mul_ps(_mm_set1_ps(scale), as_xmm);

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
    
SVectorExt vector_mul(const SVectorExt& right, const SVectorExt& left)
{
    SVectorExt result;

    result.x = right.y*left.z - right.z*left.y;
    result.y = right.z*left.x - right.x*left.z;
    result.z = right.x*left.y - right.y*left.x;
    result.w = right.w*left.w;

    return result;
}

//TODO:split SVectorExt to SVectorExt and SNormal
float scalar_mul(const SVectorExt& right, const SVectorExt& left)
{
    float result = 0.0f;

    result += right.x*left.x;
    result += right.y*left.y;
    result += right.z*left.z;

    if (fabs(right.w*left.w) > FLT_EPSILON)
        result /= (right.w*left.w);

    return result;
}

float abs(const SVectorExt& vector)
{
    return sqrtf(scalar_mul(vector, vector));
}

SVectorExt unitary(const SVectorExt& vector)
{
    SVectorExt result;

    if (fabs(vector.w) < FLT_EPSILON)
        result = vector;
    else
        result = vector*(1.0f/vector.w);

    return result; 
}

SVectorExt normal(const SVectorExt& vector)
{
    SVectorExt result; 

    float length = abs(vector);
    if (length < FLT_EPSILON)
        result = SVectorExt();
    else
        result = unitary(vector*(1.0f/length));

    result.w = 0.0f;

    return result;
}

int test_SVectorExt()
{
    SVectorExt vec1 = SVectorExt(0.5f, 6.7f, -3.4f);
    SVectorExt vec2 = SVectorExt(-4.3f, 0.1f, 2.6f, 0.5f);

    SVectorExt add = vec1 + vec2;
    SVectorExt sub = vec1 - vec2;
    SVectorExt mul_vec = vector_mul(vec1, vec2);
    float      mul_scal = scalar_mul(vec1, vec2);

    printf("vec1    %f %f %f %f\n", vec1.x, vec1.y, vec1.z, vec1.w);
    printf("vec2    %f %f %f %f\n", vec2.x, vec2.y, vec2.z, vec2.w);
    printf("add     %f %f %f %f\n", add.x, add.y, add.z, add.w);
    printf("sub     %f %f %f %f\n", sub.x, sub.y, sub.z, sub.w);
    printf("mul_vec %f %f %f %f\n", mul_vec.x, mul_vec.y, mul_vec.z, mul_vec.w);
    printf("mul_scal %f\n", mul_scal);

    return 0;
}

//} //namespace sgl

#endif //EGL_SVECTOR_H
