#ifndef SGL_CTEXTURE_H
#define SGL_CTEXTURE_H

#include <cstdio>

#include "math/SColorExt.h"

//namespace sgl {

class CTexture
{
public:
    const size_t CELL_SIZE = 16;

    CTexture();

    void read_from_bmp(FILE* bitmap_file);
    SColorExt get_color(float tex_u, float tex_v) const;

private:
    size_t dim_h_;
    size_t dim_w_;

    SColorExt* buffer_;
};

//} //namespace sgl

#endif //SGL_CTEXTURE_H
