#ifndef SGL_CPERSPECTIVE_H
#define SGL_CPERSPECTIVE_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions

#include "SVectorExt.h"
#include "SMatrixExt.h"

//namespace sgl {

class CPerspective
{
public:
    CPerspective(float near_set, float far_set);

    CPerspective             (const CPerspective&);
    CPerspective& operator = (const CPerspective&);
    CPerspective             (CPerspective&&);
    CPerspective& operator = (CPerspective&&);

    ~CPerspective();

    SMatrixExt get_matrix() const;

private:
    float near_;
    float far_;
};

CPerspective::CPerspective(float near_set, float far_set):
    near_(-near_set),
    far_ (-far_set)
{}

CPerspective::CPerspective(const CPerspective& copy_perspective):
    near_(copy_perspective.near_),
    far_ (copy_perspective.far_)
{}

CPerspective& CPerspective::operator = (const CPerspective& copy_perspective)
{
    near_ = copy_perspective.near_;
    far_  = copy_perspective.far_;

    return *this;
}

CPerspective::CPerspective(CPerspective&& move_perspective):
    near_(std::move(move_perspective.near_)),
    far_ (std::move(move_perspective.far_))
{}

CPerspective& CPerspective::operator = (CPerspective&& move_perspective)
{
    std::swap(near_, move_perspective.near_);
    std::swap(far_,  move_perspective.far_);

    return *this;
}

CPerspective::~CPerspective()
{
    near_ = far_ = 0.0f;
}

SMatrixExt CPerspective::get_matrix() const
{
    float mat33 = -(far_ + near_)/(far_ - near_);
    float mat34 = -(2*far_*near_)/(far_ - near_);

    SMatrixExt result = SMatrixExt(SVectorExt(near_, 0.0f, 0.0f, 0.0f),
                                   SVectorExt(0.0f, near_, 0.0f, 0.0f),
                                   SVectorExt(0.0f, 0.0f, mat33, mat34),
                                   SVectorExt(0.0f, 0.0f, -1.0f, 0.0f));

    return result;
}

//} //namespace sgl

#endif //SGL_CPERSPECTIVE_H
