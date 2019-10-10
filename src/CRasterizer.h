#ifndef SGL_CRASTERIZER_H
#define SGL_CRASTERIZER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

//rasterizer
#include <cmath>
#include <vector>
#include "ShaderStructures.h"
#include "SVector.h" 
#include "SColor.h" 
#include "CScreen.h"
#include "CBuffer.h"

//namespace sgl {

class CRasterizer
{
public:
    CRasterizer();

    CRasterizer             (const CRasterizer&) = delete;
    CRasterizer& operator = (const CRasterizer&) = delete;

    CRasterizer             (CRasterizer&&);
    CRasterizer& operator = (CRasterizer&&);

    ~CRasterizer();

    const std::vector<SFragment>& frag_vec() const noexcept
    {
        return frag_vec_;
    }

    void clear()
    {
        frag_vec_.clear();
    }

    void rast_line(const SVertex& beg_v, 
                   const SVertex& end_v);

    void rast_face(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v);

private:
    std::vector<SFragment> frag_vec_;
};

CRasterizer::CRasterizer():
    frag_vec_()
{}

CRasterizer::CRasterizer(CRasterizer&& move_rast):
    frag_vec_(std::move(move_rast.frag_vec_))
{}

CRasterizer& CRasterizer::operator = (CRasterizer&& move_rast)
{
    frag_vec_ = std::move(move_rast.frag_vec_);
    return *this;
}

CRasterizer::~CRasterizer()
{
    frag_vec_.clear();
}

void CRasterizer::rast_line(const SVertex& beg_v, 
                            const SVertex& end_v)
{
    size_t step_cnt = std::max(size_t(fabs(end_v.point.x - beg_v.point.x)),
                               size_t(fabs(end_v.point.y - beg_v.point.y)));
    
    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        //step_cnt to-float conversion shoul be optimized
        float ratio = (float(cur_step) / float(step_cnt));
        SVector cur_point = (beg_v.point * (1.0f - ratio) + 
                             end_v.point *         ratio);

        SColor cur_color = interpolate(beg_v.color, end_v.color, ratio);

        SFragment cur_frag = {};

        cur_frag.x = size_t(cur_point.x);
        cur_frag.y = size_t(cur_point.y);

        cur_frag.depth = cur_point.z;
        cur_frag.color = { uint8_t(255.0f*cur_color.r),
                           uint8_t(255.0f*cur_color.g),
                           uint8_t(255.0f*cur_color.b),
                           uint8_t(255.0f*cur_color.a) };

        frag_vec_.push_back(cur_frag);
    }
}

//TODO: implement lerp
void CRasterizer::rast_face(const SVertex& beg_v, 
                            const SVertex& mid_v, 
                            const SVertex& end_v)
{
    rast_line(beg_v, mid_v);
    rast_line(mid_v, end_v);
    rast_line(end_v, beg_v);
}

int test_CRasterizer()
{
    srand(time(NULL));
    
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();
    CRasterizer rasterizer = CRasterizer();

    const size_t LINE_CNT = 0x100;
    for (size_t i = 0; i < LINE_CNT; ++i)
    {
        SVertex cur_beg_v = {};
        SVertex cur_end_v = {};       

        cur_beg_v.point.x = float(rand()%CBuffer::DIM_W);
        cur_beg_v.point.y = float(rand()%CBuffer::DIM_H);

        cur_end_v.point.x = float(rand()%CBuffer::DIM_W);
        cur_end_v.point.y = float(rand()%CBuffer::DIM_H);
       
        cur_beg_v.color = SColor(float(rand()&0xFF)/255.0f,
                                 float(rand()&0xFF)/255.0f,
                                 float(rand()&0xFF)/255.0f);

        cur_end_v.color = SColor(float(rand()&0xFF)/255.0f,
                                 float(rand()&0xFF)/255.0f,
                                 float(rand()&0xFF)/255.0f);

        rasterizer.rast_line(cur_beg_v, cur_end_v);
    } 
/*
    printf("rasterizer.frag_vec().size() = %lu \n", 
            rasterizer.frag_vec().size());

    for (const auto& frag : rasterizer.frag_vec())
    {
        printf("frag {x=%lu y=%lu depth=%f color={%u %u %u %u}} \n",
                frag.x, frag.y, frag.depth,
                frag.color.r, frag.color.g, frag.color.b, frag.color.a); 
    }
*/
    buf.render(rasterizer.frag_vec());
    scr.write(buf.data(), buf.byte_size());

    return 0;
}

//} //namespace sgl 

#endif //SGL_CRASTERIZER_H
