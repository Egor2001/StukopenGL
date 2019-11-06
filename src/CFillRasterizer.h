#ifndef SGL_CFILLRASTERIZER_H
#define SGL_CFILLRASTERIZER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <ctime>

//rasterizer
#include <cmath>
#include <vector>
#include "SVertex.h"
#include "SFragment.h"
#include "math/SVector.h" 
#include "math/SColor.h" 
#include "CScreen.h"
#include "CBuffer.h"
#include "memory/CIntrinsicAllocator.h"

//namespace sgl {

template<typename FC>
class CFillRasterizer
{
public:
    using TFragmentContainer = FC;

    CFillRasterizer(float max_x_set, float max_y_set):
        max_x_(max_x_set),
        max_y_(max_y_set)
    {}

    float max_x() const noexcept { return max_x_; }
    float max_y() const noexcept { return max_y_; }

    void rasterize(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v, 
                   TFragmentContainer& frag_buf);

private:
    void fill_face(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v, 
                   TFragmentContainer& frag_buf);

    void fill_half(const SFragment& beg_f, const SFragment& end_f,
                   const SFragment& top_f,
                   TFragmentContainer& frag_buf);

    void fill_xseq(const SFragment& beg_f, const SFragment& end_f,
                   TFragmentContainer& frag_buf);

    float max_x_;
    float max_y_;
};

template<typename FC>
void 
CFillRasterizer<FC>::
rasterize(const SVertex& beg_v, 
          const SVertex& mid_v, 
          const SVertex& end_v, 
          TFragmentContainer& frag_buf)
{
    fill_face(beg_v, mid_v, end_v, frag_buf);
}

template<typename FC>
void 
CFillRasterizer<FC>::
fill_face(const SVertex& beg_v, 
          const SVertex& mid_v, 
          const SVertex& end_v,
          TFragmentContainer& frag_buf)
{
    SFragment beg_f = ::to_fragment(beg_v);
    SFragment mid_f = ::to_fragment(mid_v);
    SFragment end_f = ::to_fragment(end_v);

    if (beg_f.point.y < mid_f.point.y) std::swap(beg_f, mid_f);
    if (mid_f.point.y < end_f.point.y) std::swap(mid_f, end_f);
    if (beg_f.point.y < mid_f.point.y) std::swap(beg_f, mid_f);

    if (fabs(beg_f.point.y - end_f.point.y) < 1.0f)
    {
        fill_xseq(beg_f, mid_f, frag_buf);
        fill_xseq(end_f, mid_f, frag_buf);
    }
    else
    {
        float ratio = (beg_f.point.y - mid_f.point.y) / 
                      (beg_f.point.y - end_f.point.y);

        SFragment div_f = ::interpolate(beg_f, end_f, ratio);

        fill_half(mid_f, div_f, beg_f, frag_buf);
        fill_half(mid_f, div_f, end_f, frag_buf);
    }
}

template<typename FC>
void 
CFillRasterizer<FC>::
fill_half(const SFragment& beg_f, 
          const SFragment& end_f,
          const SFragment& top_f,
          TFragmentContainer& frag_buf)
{
    float length = fabs(top_f.point.y - beg_f.point.y);
    size_t step_cnt = size_t(ceilf(length));
    
    if (length < 0.5f)
    {
        fill_xseq(beg_f, end_f, frag_buf);
    }
    else
    {
        for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
        {
            float ratio = float(cur_step)/length;
            fill_xseq(::interpolate(top_f, beg_f, ratio), 
                      ::interpolate(top_f, end_f, ratio),
                      frag_buf);
        }
    }
}

template<typename FC>
void 
CFillRasterizer<FC>::
fill_xseq(const SFragment& beg_f, 
          const SFragment& end_f,
          TFragmentContainer& frag_buf)
{
    float length = fabs(beg_f.point.x - end_f.point.x);
    size_t step_cnt = size_t(ceilf(length));

    if (length < 0.5f)
    {
        frag_buf.push_back(::interpolate(beg_f, end_f, 0.5f));
    }
    else
    {
        for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
        {
            float ratio = float(cur_step)/length;
            frag_buf.push_back(::interpolate(beg_f, end_f, ratio));
        }
    }
}

int test_CFillRasterizer()
{
    srand(time(NULL));
    
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();
    
    auto rasterizer = 
        CFillRasterizer<CIntrinsicVector<SFragment>>
            (float(CBuffer::DIM_W), float(CBuffer::DIM_H));

    CIntrinsicVector<SVertex>   vert_vec;
    CIntrinsicVector<SFragment> frag_vec;

    const size_t FACE_CNT = 0x10;
    SVertex vert_arr[3];
    for (size_t i = 0; i < FACE_CNT*3; ++i)
    {
        for (size_t j = 0; j < 3; ++j)
        {
            vert_arr[i].point.x = float(rand()%CBuffer::DIM_W);
            vert_arr[i].point.y = float(rand()%CBuffer::DIM_H);

            vert_arr[i].color = SColor{ float(rand()&0xFF)/255.0f,
                                        float(rand()&0xFF)/255.0f,
                                        float(rand()&0xFF)/255.0f };
        }

        rasterizer.rasterize(vert_arr[0], vert_arr[1], vert_arr[2], 
                             frag_vec);
    } 

    buf.render(frag_vec);
    scr.write(buf.data(), buf.byte_size());

    return 0;
}

//} //namespace sgl 

#endif //SGL_CFILLRASTERIZER_H
