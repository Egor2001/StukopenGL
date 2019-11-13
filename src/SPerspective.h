#ifndef SGL_SPERSPECTIVE_H
#define SGL_SPERSPECTIVE_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions

#include "math/SVectorExt.h"
#include "math/SMatrixExt.h"

//namespace sgl {

struct SPerspective
{
public:
    SMatrixExt get_matrix() const;

    float near;
    float far;
};

SMatrixExt SPerspective::get_matrix() const
{
    float mat33 = -(far + near)/(far - near);
    float mat34 = -(2*far*near)/(far - near);

    SMatrixExt result = SMatrixExt(SVectorExt(near, 0.0f, 0.0f, 0.0f),
                                   SVectorExt(0.0f, near, 0.0f, 0.0f),
                                   SVectorExt(0.0f, 0.0f, mat33, mat34),
                                   SVectorExt(0.0f, 0.0f, -1.0f, 0.0f));

    return result;
}

//} //namespace sgl

#endif //SGL_SPERSPECTIVE_H
