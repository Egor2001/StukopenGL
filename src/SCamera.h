#ifndef SGL_SCAMERA_H
#define SGL_SCAMERA_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

#include "math/SVectorExt.h"
#include "math/SMatrixExt.h"

//namespace sgl {

struct SCamera
{
public:
    SMatrixExt get_matrix() const;

    SVector pos;
    SVector dir;
    SVector up;
};

SMatrixExt SCamera::get_matrix() const
{
    SMatrixExt result = SMatrixExt();

    SVector x_vec = ::mul(up, dir);
    SVector y_vec = ::mul(dir, x_vec);
    SVector z_vec = dir;
    result *= ::sgl_new_basis_mtx(::normal(x_vec), 
                                  ::normal(y_vec), 
                                  ::normal(z_vec));

    SVector shift_vec = -pos; 
    result *= ::sgl_translate_mtx(::extend(shift_vec));

    return result;
}

int test_SCamera()
{
    SCamera cam = SCamera
    {
        .pos = SVector{  1.0f,  1.0f,  1.0f },
        .dir = SVector{ -1.0f, -1.0f, -1.0f },
        .up  = SVector{ 0.0f, 1.0f, 0.0f }
    };

    SMatrixExt mtx = cam.get_matrix();
    for (size_t y = 0; y < 4; ++y)
        printf("%.3f %.3f %.3f %.3f \n", 
                mtx[y][0], mtx[y][1], mtx[y][2], mtx[y][3]);

    float coord = 1.0f - sqrtf(3.0f)/3.0f;
    SVectorExt vec = SVectorExt(coord, coord, coord);

    printf("initial vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);
    
    vec = mtx*vec;

    printf("camera vec: %.3f %.3f %.3f %.3f \n", 
            vec.x, vec.y, vec.z, vec.w);

    return 0;
}

//} //namespace sgl

#endif //SGL_SCAMERA_H
