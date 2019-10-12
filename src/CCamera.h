#ifndef SGL_CCAMERA_H
#define SGL_CCAMERA_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#include "SVector.h"
#include "SMatrix.h"

//namespace sgl {

class CCamera
{
public:
    CCamera();
    CCamera(const SVector& pos_set, const SVector& dir_set, 
            const SVector& up_set = SVector(0.0f, 1.0f, 0.0f));

    CCamera             (const CCamera&) = delete;
    CCamera& operator = (const CCamera&) = delete;
    CCamera             (CCamera&&);
    CCamera& operator = (CCamera&&);

    ~CCamera();

    SMatrix get_matrix() const;

private:
    SVector pos_;
    SVector dir_;
    SVector up_;
};

CCamera::CCamera():
    pos_(), dir_(),
    up_(0.0f, 1.0f, 0.0f)
{}

CCamera::CCamera(const SVector& pos_set, const SVector& dir_set, 
                 const SVector& up_set):
    pos_(pos_set), 
    dir_(dir_set),
    up_ (up_set)
{}

CCamera::CCamera(CCamera&& move_camera):
    pos_(std::move(move_camera.pos_)), 
    dir_(std::move(move_camera.dir_)),
    up_ (std::move(move_camera.up_))
{}

CCamera& CCamera::operator = (CCamera&& move_camera)
{
    pos_ = std::move(move_camera.pos_); 
    dir_ = std::move(move_camera.dir_);
    up_  = std::move(move_camera.up_);

    return *this;
}

CCamera::~CCamera()
{
    pos_ = SVector();
    dir_ = SVector();
    up_  = SVector();
}

SMatrix CCamera::get_matrix() const
{
    SVector x_vec = vector_mul(up_, dir_);
    SVector y_vec = vector_mul(dir_, x_vec);
    SVector z_vec = dir_;
    SVector shift_vec = unitary(-pos_); 

    SMatrix result = SMatrix();

    result *= SMatrix(normal(x_vec), normal(y_vec), normal(z_vec));
    result *= sgl_translate_mtx(shift_vec);

    return result;
}

int test_CCamera()
{
    CCamera cam = CCamera(SVector( 1.0f,  1.0f,  1.0f),
                          SVector(-1.0f, -1.0f, -1.0f));

    SMatrix mtx = cam.get_matrix();
    for (size_t y = 0; y < 4; ++y)
        printf("%.3f %.3f %.3f %.3f \n", 
                mtx[y][0], mtx[y][1], mtx[y][2], mtx[y][3]);

    float coord = 1.0f - sqrtf(3.0f)/3.0f;
    SVector vec = SVector(coord, coord, coord);
    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    
    vec = mtx*vec;

    printf("camera vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

//} //namespace sgl

#endif //SGL_CCAMERA_H
