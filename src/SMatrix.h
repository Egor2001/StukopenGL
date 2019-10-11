#ifndef SGL_SMATRIX_H
#define SGL_SMATRIX_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <utility>

#define _USE_MATH_DEFINES
#include <cmath>

#include "SVector.h"

//namespace sgl {

struct SMatrix;

SMatrix transposed(const SMatrix& mtx);
SMatrix operator * (const SMatrix& mtx, float scale);

SMatrix operator + (const SMatrix& mtx, const SMatrix& add);
SMatrix operator - (const SMatrix& mtx, const SMatrix& sub);
SMatrix operator * (const SMatrix& mtx, const SMatrix& mul);

SVector operator * (const SMatrix& mtx, const SVector& vec);

SMatrix sgl_translate_mtx(const SVector& vec);
SMatrix sgl_new_basis_mtx(const SVector& new_x, 
                          const SVector& new_y, 
                          const SVector& new_z);

struct SMatrix
{
    explicit SMatrix(float scale = 1.0f);
    explicit SMatrix(const float mtx_set[4][4]);
    
    SMatrix(const SVector& x_vec, const SVector& y_vec, const SVector& z_vec, 
            const SVector& w_vec = SVector(0.0f, 0.0f, 0.0f, 1.0f));

    SMatrix             (const SMatrix&);
    SMatrix& operator = (const SMatrix&);
    SMatrix             (SMatrix&&);
    SMatrix& operator = (SMatrix&&);
    
    ~SMatrix();

    float* operator [] (size_t idx) noexcept
    {
        return mtx[idx];
    }

    const float* operator [] (size_t idx) const noexcept
    {
        return mtx[idx];
    }

    SMatrix& operator *= (float scale);

    SMatrix& operator += (const SMatrix& add);
    SMatrix& operator -= (const SMatrix& sub);
    SMatrix& operator *= (const SMatrix& mul);

    float mtx[4][4];
};

SMatrix::SMatrix(float scale):
    mtx{{scale, 0.0f, 0.0f, 0.0f},
        {0.0f, scale, 0.0f, 0.0f},
        {0.0f, 0.0f, scale, 0.0f},
        {0.0f, 0.0f, 0.0f, scale}}
{}

SMatrix::SMatrix(const float mtx_set[4][4]):
    mtx{}
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] = mtx_set[y][x];
}

SMatrix::SMatrix(const SVector& x_vec, 
                 const SVector& y_vec, 
                 const SVector& z_vec, 
                 const SVector& w_vec):
    mtx{{x_vec.x, x_vec.y, x_vec.z, x_vec.w},
        {y_vec.x, y_vec.y, y_vec.z, y_vec.w},
        {z_vec.x, z_vec.y, z_vec.z, z_vec.w},
        {w_vec.x, w_vec.y, w_vec.z, w_vec.w}}
{}

SMatrix::SMatrix(const SMatrix& copy_matrix):
    mtx{}
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] = copy_matrix.mtx[y][x];
}

SMatrix& SMatrix::operator = (const SMatrix& copy_matrix)
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] = copy_matrix.mtx[y][x];

    return *this;
}

SMatrix::SMatrix(SMatrix&& move_matrix):
    mtx{}
{
    std::swap(mtx, move_matrix.mtx);
}

SMatrix& SMatrix::operator = (SMatrix&& move_matrix)
{
    std::swap(mtx, move_matrix.mtx);

    return *this;
}

SMatrix::~SMatrix()
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] = 0.0f; 

    for (size_t i = 0; i < 4; ++i)
        mtx[i][i] = 1.0f;
}

SMatrix& SMatrix::operator *= (float scale)
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] *= scale; 

    return *this;
}

SMatrix& SMatrix::operator += (const SMatrix& add)
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] += add.mtx[y][x];

    return *this;
}

SMatrix& SMatrix::operator -= (const SMatrix& sub)
{
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        mtx[y][x] -= sub.mtx[y][x];

    return *this;
}

SMatrix& SMatrix::operator *= (const SMatrix& mul)
{
    SMatrix result = SMatrix(0.0f);

    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        for (size_t i = 0; i < 4; ++i)
            result[y][x] += mtx[y][i] * mul.mtx[i][x];

    return (*this = result);
}

SMatrix transposed(const SMatrix& mtx)
{
    SMatrix result = mtx;
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        result[y][x] = mtx[x][y]; 

    return result;
}

SMatrix operator * (const SMatrix& mtx, float scale)
{
    SMatrix result = mtx;
    return (result *= scale);
}

SMatrix operator + (const SMatrix& mtx, const SMatrix& add)
{
    SMatrix result = mtx;
    return (result += add);
}

SMatrix operator - (const SMatrix& mtx, const SMatrix& sub)
{
    SMatrix result = mtx;
    return (result -= sub);
}

SMatrix operator * (const SMatrix& mtx, const SMatrix& mul)
{
    SMatrix result = mtx;
    return (result *= mul);
}   

SVector operator * (const SMatrix& mtx, const SVector& vec)
{
    SVector result = SVector(0.0f, 0.0f, 0.0f, 0.0f);
    for (size_t y = 0; y < 4; ++y)
    for (size_t i = 0; i < 4; ++i)
        result[y] += mtx[y][i] * vec[i]; 

    return result;
}

SMatrix sgl_translate_mtx(const SVector& vec)
{
    SMatrix result = SMatrix(SVector(1.0f, 0.0f, 0.0f, vec.x), 
                             SVector(0.0f, 1.0f, 0.0f, vec.y), 
                             SVector(0.0f, 0.0f, 1.0f, vec.z), 
                             SVector(0.0f, 0.0f, 0.0f, vec.w));

    return result;
}

SMatrix sgl_new_basis_mtx(const SVector& new_x, 
                          const SVector& new_y, 
                          const SVector& new_z)
{
    SMatrix result = SMatrix(new_x, new_y, new_z);

    return result;
}

int test_SMatrix()
{
    float ang = float(M_PI / 12.0f);

    SVector vec_x = SVector(cosf(ang), -sinf(ang), 0.0f, 0.0f);
    SVector vec_y = SVector(sinf(ang),  cosf(ang), 0.0f, 0.0f);
    SVector vec_z = SVector(0.0f,       0.0f,      1.0f, 0.0f);

    SMatrix mtx = sgl_new_basis_mtx(vec_x, vec_y, vec_z); 
    mtx = mtx*mtx*mtx;

    SVector vec = SVector(sqrtf(2.0f), sqrtf(2.0f), 2.0f);

    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    vec = mtx*vec;
    printf("rotated vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

//} //namespace sgl

#endif //SGL_SMATRIX_H
