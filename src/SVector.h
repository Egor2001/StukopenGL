#ifndef SGL_SVECTOR_H
#define SGL_SVECTOR_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//vector
#include <cmath>
#include <cfloat>

//namespace sgl {

struct SVector;

SVector operator + (const SVector& right, const SVector& left);
SVector operator - (const SVector& right, const SVector& left);
SVector operator * (const SVector& vector, float scale); 
    
SVector unitary(const SVector& vector);

SVector vector_mul(const SVector& right, const SVector& left);
float   scalar_mul(const SVector& right, const SVector& left);

struct SVector
{
public:
    SVector():
        x(0.0f), y(0.0f), z(0.0f), w(1.0f)
    {}

    SVector(float x_set, float y_set, float z_set, float w_set = 1.0f):
        x(x_set), y(y_set), z(z_set), w(w_set)
    {}

    float& operator [] (size_t idx) noexcept
    {
        switch (idx)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return w;                 
        }
    }
 
    const float& operator [] (size_t idx) const noexcept
    {
        switch (idx)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return w;                 
        }
    }
   
    SVector& operator += (const SVector& add)
    {
        x = x*add.w + add.x*w; 
        y = y*add.w + add.y*w; 
        z = z*add.w + add.z*w; 
        w *= add.w;

        return *this;
    }
    
    SVector& operator -= (const SVector& sub)
    {
        x = x*sub.w - sub.x*w; 
        y = y*sub.w - sub.y*w; 
        z = z*sub.w - sub.z*w; 
        w *= sub.w;

        return *this;
    }
    
    SVector& operator *= (float scale)
    {
        x *= scale; 
        y *= scale; 
        z *= scale; 

        return *this;
    }

    float x, y, z, w;
};

SVector operator + (const SVector& right, const SVector& left)
{
    SVector result = right;
    return (result += left);    
}

SVector operator - (const SVector& right, const SVector& left)
{
    SVector result = right;
    return (result -= left);    
}

SVector operator * (const SVector& vector, float scale) 
{
    SVector result = vector;
    return (result *= scale);    
}
    
SVector unitary(const SVector& vector)
{
    SVector result;

    if (fabs(vector.w) < FLT_EPSILON)
        result = SVector(FLT_MAX, FLT_MAX, FLT_MAX, 1.0f);
    else
        result = vector*(1.0f/vector.w);

    return result; 
}

SVector vector_mul(const SVector& right, const SVector& left)
{
    SVector result = {};

    result.x = right.y*left.z - right.z*left.y;
    result.y = right.z*left.x - right.x*left.z;
    result.z = right.x*left.y - right.y*left.x;
    result.w = right.w*left.w;

    return result;
}

float scalar_mul(const SVector& right, const SVector& left)
{
    float result = 0.0f;

    result += right.x*left.x;
    result += right.y*left.y;
    result += right.z*left.z;

    if (fabs(right.w*left.w) > FLT_EPSILON)
        result /= right.w*left.w;
    else
        result = FLT_MAX;

    return result;
}

int test_SVector()
{
    SVector vec1 = SVector(0.5f, 6.7f, -3.4f);
    SVector vec2 = SVector(-4.3f, 0.1f, 2.6f, 0.5f);

    SVector add = vec1 + vec2;
    SVector sub = vec1 - vec2;
    SVector mul_vec = vector_mul(vec1, vec2);
    float   mul_scal = scalar_mul(vec1, vec2);

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
