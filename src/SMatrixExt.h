#ifndef SGL_SMATRIXEXT_H
#define SGL_SMATRIXEXT_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <immintrin.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "SVectorExt.h"

//namespace sgl {

struct SMatrixExt;

SMatrixExt transposed(const SMatrixExt& mtx);
SMatrixExt operator * (const SMatrixExt& mtx, float scale);

SMatrixExt operator + (const SMatrixExt& mtx, const SMatrixExt& add);
SMatrixExt operator - (const SMatrixExt& mtx, const SMatrixExt& sub);
SMatrixExt operator * (const SMatrixExt& mtx, const SMatrixExt& mul);

SVectorExt operator * (const SMatrixExt& mtx, const SVectorExt& vec);

SMatrixExt sgl_translate_mtx(const SVector& vec);
SMatrixExt sgl_new_basis_mtx(const SVector& new_x, 
                             const SVector& new_y, 
                             const SVector& new_z);

struct SMatrixExt
{
    explicit SMatrixExt(float scale = 1.0f);
    explicit SMatrixExt(const float mtx_set[4][4]);
    
    SMatrixExt(const SVectorExt& x_vec, 
               const SVectorExt& y_vec, 
               const SVectorExt& z_vec, 
               const SVectorExt& w_vec = SVectorExt());

    float* operator [] (size_t idx) noexcept
    {
        return mtx[idx];
    }

    const float* operator [] (size_t idx) const noexcept
    {
        return mtx[idx];
    }

    SMatrixExt& operator *= (float scale);

    SMatrixExt& operator += (const SMatrixExt& add);
    SMatrixExt& operator -= (const SMatrixExt& sub);
    SMatrixExt& operator *= (const SMatrixExt& mul);

    union 
    {
        __m128 as_xmm_vec[4];
        float         mtx[4][4];
    };
};

SMatrixExt::SMatrixExt(float scale):
    as_xmm_vec{ _mm_setr_ps(scale, 0.0f, 0.0f, 0.0f),
                _mm_setr_ps(0.0f, scale, 0.0f, 0.0f),
                _mm_setr_ps(0.0f, 0.0f, scale, 0.0f),
                _mm_setr_ps(0.0f, 0.0f, 0.0f, scale) }
{}

SMatrixExt::SMatrixExt(const float mtx_set[4][4]):
    mtx{}
{
    for (size_t y = 0; y < 4; ++y)
        as_xmm_vec[y] = _mm_loadu_ps(mtx_set[y]);
}

SMatrixExt::SMatrixExt(const SVectorExt& x_vec, 
                       const SVectorExt& y_vec, 
                       const SVectorExt& z_vec, 
                       const SVectorExt& w_vec):
    as_xmm_vec{ x_vec.as_xmm, y_vec.as_xmm, z_vec.as_xmm, w_vec.as_xmm }
{}

SMatrixExt& SMatrixExt::operator *= (float scale)
{
    for (size_t y = 0; y < 4; ++y)
        as_xmm_vec[y] = _mm_mul_ps(as_xmm_vec[y], _mm_set1_ps(scale));

    return *this;
}

SMatrixExt& SMatrixExt::operator += (const SMatrixExt& add)
{
    for (size_t y = 0; y < 4; ++y)
        as_xmm_vec[y] = _mm_add_ps(as_xmm_vec[y], add.as_xmm_vec[y]);

    return *this;
}

SMatrixExt& SMatrixExt::operator -= (const SMatrixExt& sub)
{
    for (size_t y = 0; y < 4; ++y)
        as_xmm_vec[y] = _mm_sub_ps(as_xmm_vec[y], sub.as_xmm_vec[y]);

    return *this;
}

SMatrixExt& SMatrixExt::operator *= (const SMatrixExt& mul)
{
    SMatrixExt before = *this;
    SMatrixExt transp = transposed(mul);
    
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        as_xmm_vec[y][x] = _mm_cvtss_f32(_mm_dp_ps(before.as_xmm_vec[y], 
                                                   transp.as_xmm_vec[x], 
                                                   0b11111111));

    return *this;
}

SMatrixExt transposed(const SMatrixExt& mtx)
{
    SMatrixExt result = mtx;
    for (size_t y = 0; y < 4; ++y)
    for (size_t x = 0; x < 4; ++x)
        result[y][x] = mtx[x][y]; 

    return result;
}

SMatrixExt operator * (const SMatrixExt& mtx, float scale)
{
    SMatrixExt result = mtx;
    return (result *= scale);
}

SMatrixExt operator + (const SMatrixExt& mtx, const SMatrixExt& add)
{
    SMatrixExt result = mtx;
    return (result += add);
}

SMatrixExt operator - (const SMatrixExt& mtx, const SMatrixExt& sub)
{
    SMatrixExt result = mtx;
    return (result -= sub);
}

SMatrixExt operator * (const SMatrixExt& mtx, const SMatrixExt& mul)
{
    SMatrixExt result = mtx;
    return (result *= mul);
}   
//zdes' ya dolgo kuril manual
SVectorExt operator * (const SMatrixExt& mtx, const SVectorExt& vec)
{
    SVectorExt result;
    for (size_t y = 0; y < 4; ++y)
        result[y] = _mm_cvtss_f32(_mm_dp_ps(mtx.as_xmm_vec[y], 
                                            vec.as_xmm, 0b11111111));

    return result;
}

SMatrixExt sgl_translate_mtx(const SVectorExt& vec)
{
    SMatrixExt result = SMatrixExt(SVectorExt(1.0f, 0.0f, 0.0f, vec.x), 
                                   SVectorExt(0.0f, 1.0f, 0.0f, vec.y), 
                                   SVectorExt(0.0f, 0.0f, 1.0f, vec.z), 
                                   SVectorExt(0.0f, 0.0f, 0.0f, vec.w));

    return result;
}

SMatrixExt sgl_new_basis_mtx(const SVector& new_x, 
                             const SVector& new_y, 
                             const SVector& new_z)
{
    SMatrixExt result = SMatrixExt(SVectorExt(new_x.x, new_x.y, new_x.z, 0.0f),
                                   SVectorExt(new_y.x, new_y.y, new_y.z, 0.0f),
                                   SVectorExt(new_z.x, new_z.y, new_z.z, 0.0f));
    
    return result;
}

int test_SMatrixExt()
{
    float ang = float(M_PI / 12.0f); 
    SVector vec_x = SVector(cosf(ang), -sinf(ang), 0.0f);
    SVector vec_y = SVector(sinf(ang),  cosf(ang), 0.0f);
    SVector vec_z = SVector(0.0f,       0.0f,      1.0f);

    SMatrixExt mtx = sgl_new_basis_mtx(vec_x, vec_y, vec_z); 

    for (size_t y = 0; y < 4; ++y)
        printf("%.3f %.3f %.3f %.3f \n", 
                mtx[y][0], mtx[y][1], mtx[y][2], mtx[y][3]);

    mtx = mtx*mtx*mtx;

    for (size_t y = 0; y < 4; ++y)
        printf("%.3f %.3f %.3f %.3f \n", 
                mtx[y][0], mtx[y][1], mtx[y][2], mtx[y][3]);

    SVectorExt vec = SVectorExt(sqrtf(2.0f), sqrtf(2.0f), 2.0f);

    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    vec = mtx*vec;
    printf("rotated vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

//} //namespace sgl

#endif //SGL_SMATRIXEXT_H
