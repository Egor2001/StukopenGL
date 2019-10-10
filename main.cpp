//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

//double including all tested files to check collisions
#include "src/SVector.h"
#include "src/SVector.h"
#include "src/SColor.h"
#include "src/SColor.h" 
#include "src/CRasterizer.h"
#include "src/CRasterizer.h"
#include "src/CScreen.h"
#include "src/CScreen.h"
#include "src/CBuffer.h"
#include "src/CBuffer.h"
#include "src/CObject.h"
#include "src/CObject.h"

struct SMatrix;

SMatrix transposed(const SMatrix& mtx);
SMatrix operator * (float scale);

SMatrix operator + (const SMatrix& mtx, const SMatrix& add);
SMatrix operator - (const SMatrix& mtx, const SMatrix& sub);
SMatrix operator * (const SMatrix& mtx, const SMatrix& mul);

SVector operator * (const SMatrix& mtx, const SVector& vec);

struct SMatrix
{
    explicit SMatrix(float scale = 1.0f);
    explicit SMatrix(const float mtx_set[4][4]);

    SMatrix             (const SMatrix&);
    SMatrix& operator = (const SMatrix&);
    SMatrix             (SMatrix&&);
    SMatrix& operator = (SMatrix&&);
    
    ~SMatrix();

    float[] operator [] (size_t idx) noexcept
    {
        return mtx[idx];
    }

    const float[] operator [] (size_t idx) const noexcept
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
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        mtx[y][x] = mtx_set[y][x];
}

SMatrix::SMatrix(const SMatrix& copy_matrix):
    mtx{}
{
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        mtx[y][x] = copy_matrix.mtx[y][x];
}

SMatrix& SMatrix::operator = (const SMatrix& copy_matrix)
{
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
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

~SMatrix()
{
    mtx = {};
}

SMatrix& SMatrix::operator *= (float scale)
{
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        mtx[y][x] *= scale; 

    return *this;
}

SMatrix& SMatrix::operator += (const SMatrix& add)
{
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        mtx[y][x] += add.mtx[y][x];

    return *this;
}

SMatrix& SMatrix::operator -= (const SMatrix& sub)
{
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        mtx[y][x] -= sub.mtx[y][x];

    return *this;
}

SMatrix& SMatrix::operator *= (const SMatrix& mul)
{
    SMatrix result = SMatrix(0.0f);

    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        for (size_t i = 0; i < 4; ++i)
            result[y][x] += mtx[y][i] * mul.mtx[i][x];

    return (*this = result);
}

SMatrix transposed(const SMatrix& mtx)
{
    SMatrix result = mtx;
    for (size_t x = 0; x < 4; ++x)
    for (size_t y = 0; y < 4; ++y)
        result[y][x] = mtx[x][y]; 

    return result;
}

SMatrix operator * (float scale)
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

SVector operator * (const SMatrix& mtx, const SVector& vec);
{
    SVector result = SVector(0.0f, 0.0f, 0.0f, 0.0f);
    for (size_t y = 0; y < 4; ++y)
    for (size_t i = 0; i < 4; ++i)
        result[y] += mtx[y][i] * vec[i]; 

    return result;
}

int main()
{
    return 0;
}
