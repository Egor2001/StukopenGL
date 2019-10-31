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

SVector operator - (const SVector&);

SVector operator + (const SVector&, const SVector&);
SVector operator - (const SVector&, const SVector&);
SVector operator * (const SVector&, float);

float   dot(const SVector&, const SVector&);
SVector mul(const SVector&, const SVector&);
float   abs(const SVector&);

SVector normal(const SVector&);

struct SVector
{
public:
//    SVector();
//    SVector(float x_set, float y_set, float z_set);

    SVector& operator += (const SVector& add); 
    SVector& operator -= (const SVector& sub); 
    SVector& operator *= (float scale); 

    SVector& normalize();

    union
    {
        struct { float x, y, z; };
        float vec[3];
    };
};

//SVector::SVector():
//    x(0.0f), y(0.0f), z(0.0f)
//{}

//SVector::SVector(float x_set, float y_set, float z_set):
//    x(x_set), y(y_set), z(z_set)
//{}

SVector& SVector::operator += (const SVector& add) 
{
    for (size_t i = 0; i < 3; ++i)
        vec[i] += add.vec[i];

    return *this;
}

SVector& SVector::operator -= (const SVector& sub) 
{
    for (size_t i = 0; i < 3; ++i)
        vec[i] -= sub.vec[i];

    return *this;
}

SVector& SVector::operator *= (float scale)
{
    for (size_t i = 0; i < 3; ++i)
        vec[i] *= scale;

    return *this;
}

SVector& SVector::normalize()
{
    float len = sqrtf(x*x + y*y + z*z);
    if (len > FLT_EPSILON)
        return ((*this) *= (1.0f/len));

    return *this;
}

SVector operator - (const SVector& vec)
{
    return SVector{ -vec.x, -vec.y, -vec.z };
}

SVector operator + (const SVector& lhs, const SVector& rhs)
{
    SVector result = lhs;
    return (result + rhs);
}

SVector operator - (const SVector& rhs, const SVector& lhs)
{
    SVector result = lhs;
    return (result -= rhs);
}

SVector operator * (const SVector& vec, float scale)
{
    SVector result = vec;
    return (result *= scale);
}

float dot(const SVector& lhs, const SVector& rhs)
{
    float result = lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;

    return result;
}

SVector mul(const SVector& lhs, const SVector& rhs)
{
    SVector result = SVector{};

    result.x = rhs.y*lhs.z - rhs.z*lhs.y;
    result.y = rhs.z*lhs.x - rhs.x*lhs.z;
    result.z = rhs.x*lhs.y - rhs.y*lhs.x;

    return result;
}

float abs(const SVector& vec)
{
    float result = sqrtf(dot(vec, vec));

    return result;
}

SVector normal(const SVector& vec)
{
    SVector result = vec*(1.0f/abs(vec));

    return result;
}

int test_SVector()
{
    SVector vec1 = SVector{0.5f, 6.7f, -3.4f};
    SVector vec2 = SVector{-4.3f, 0.1f, 2.6f};

    SVector add = vec1 + vec2;
    SVector sub = vec1 - vec2;
    SVector mul_vec = mul(vec1, vec2);
    float   mul_dot = dot(vec1, vec2);

    printf("vec1    %f %f %f\n", vec1.x, vec1.y, vec1.z);
    printf("vec2    %f %f %f\n", vec2.x, vec2.y, vec2.z);
    printf("add     %f %f %f\n", add.x, add.y, add.z);
    printf("sub     %f %f %f\n", sub.x, sub.y, sub.z);
    printf("mul_vec %f %f %f\n", mul_vec.x, mul_vec.y, mul_vec.z);
    printf("mul_dot %f\n", mul_dot);

    return 0;
}

//} //namespace sgl

#endif //EGL_SVECTOR_H
