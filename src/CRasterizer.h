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
#include "math/SVector.h" 
#include "math/SColor.h" 
#include "CScreen.h"
#include "CBuffer.h"
#include "memory/CIntrinsicAllocator.h"

//namespace sgl {

class CRasterizer
{
public:
    CRasterizer(float max_x_set, float max_y_set);

    CRasterizer             (const CRasterizer&) = delete;
    CRasterizer& operator = (const CRasterizer&) = delete;

    CRasterizer             (CRasterizer&&);
    CRasterizer& operator = (CRasterizer&&);

    ~CRasterizer();

    const CIntrinsicVector<SFragment>& frag_vec() const noexcept
    {
        return frag_vec_;
    }

    void clear()
    {
//        vert_vec_.clear();
        frag_vec_.clear();
    }

    void rast_line(const SVertex& beg_v, 
                   const SVertex& end_v);

    void rast_face(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v);

    void fill_face(const SVertex v_arr[3]);

    void fill_half(const SFragment& beg_f, const SFragment& end_f,
                   const SFragment& top_f);

    void fill_xseq(const SFragment& beg_f, const SFragment& end_f);

private:
//    CIntrinsicVector<SVertex>   vert_vec_;
    CIntrinsicVector<SFragment> frag_vec_;

    float max_x_;
    float max_y_;
};

CRasterizer::CRasterizer(float max_x_set, float max_y_set):
//    vert_vec_(),
    frag_vec_(),
    max_x_(max_x_set),
    max_y_(max_y_set)
{}

CRasterizer::CRasterizer(CRasterizer&& move_rast):
//    vert_vec_(std::move(move_rast.vert_vec_)),
    frag_vec_(std::move(move_rast.frag_vec_)),
    max_x_   (std::move(move_rast.max_x_)),
    max_y_   (std::move(move_rast.max_y_))
{}

CRasterizer& CRasterizer::operator = (CRasterizer&& move_rast)
{
//    std::swap(vert_vec_, move_rast.vert_vec_);
    std::swap(frag_vec_, move_rast.frag_vec_);
    std::swap(max_x_,    move_rast.max_x_);
    std::swap(max_y_,    move_rast.max_y_);

    return *this;
}

CRasterizer::~CRasterizer()
{
    max_x_ = max_y_ = 0;

    frag_vec_.clear();
//    vert_vec_.clear();
}

void CRasterizer::rast_line(const SVertex& beg_v, 
                            const SVertex& end_v)
{
    SFragment beg_f = ::to_fragment(beg_v);
    SFragment end_f = ::to_fragment(end_v);

    size_t step_cnt = std::max(size_t(fabs(end_f.point.x - 
                                           beg_f.point.x)),
                               size_t(fabs(end_f.point.y - 
                                           beg_f.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        frag_vec_.push_back(::interpolate(beg_f, end_f, ratio));
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

void CRasterizer::fill_face(const SVertex v_arr[3])
{
    SFragment beg_f = ::to_fragment(v_arr[0]);
    SFragment mid_f = ::to_fragment(v_arr[1]);
    SFragment end_f = ::to_fragment(v_arr[2]);

    if (beg_f.point.y < mid_f.point.y) std::swap(beg_f, mid_f);
    if (mid_f.point.y < end_f.point.y) std::swap(mid_f, end_f);
    if (beg_f.point.y < mid_f.point.y) std::swap(beg_f, mid_f);

    if (fabs(beg_f.point.y - end_f.point.y) < 1.0f)
    {
        fill_xseq(beg_f, mid_f);
        fill_xseq(end_f, mid_f);
    }
    else
    {
        float ratio = (beg_f.point.y - mid_f.point.y) / 
                      (beg_f.point.y - end_f.point.y);

        SFragment div_f = ::interpolate(beg_f, end_f, ratio);

        fill_half(mid_f, div_f, beg_f);
        fill_half(mid_f, div_f, end_f);
    }
}

void CRasterizer::fill_half(const SFragment& beg_f, 
                            const SFragment& end_f,
                            const SFragment& top_f)
{
    float length = fabs(top_f.point.y - beg_f.point.y);
    size_t step_cnt = size_t(ceilf(length));
    
    if (length < 0.5f)
    {
        fill_xseq(beg_f, end_f);
    }
    else
    {
        for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
        {
            float ratio = float(cur_step)/length;
            fill_xseq(::interpolate(top_f, beg_f, ratio), 
                      ::interpolate(top_f, end_f, ratio));
        }
    }
}

void CRasterizer::fill_xseq(const SFragment& beg_f, 
                            const SFragment& end_f)
{
    float length = fabs(beg_f.point.x - end_f.point.x);
    size_t step_cnt = size_t(ceilf(length));

    if (length < 0.5f)
    {
        frag_vec_.push_back(::interpolate(beg_f, end_f, 0.5f));
    }
    else
    {
        for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
        {
            float ratio = float(cur_step)/length;
            frag_vec_.push_back(::interpolate(beg_f, end_f, ratio));
        }
    }
}

int test_CRasterizer()
{
    srand(time(NULL));
    
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();
    CRasterizer rasterizer = CRasterizer(float(CBuffer::DIM_W),
                                         float(CBuffer::DIM_H));

    const size_t FACE_CNT = 0x10;
    SVertex vert_arr[3];
    for (size_t i = 0; i < FACE_CNT; ++i)
    {
        for (size_t i = 0; i < 3; ++i)
        {
            vert_arr[i].point.x = float(rand()%CBuffer::DIM_W);
            vert_arr[i].point.y = float(rand()%CBuffer::DIM_H);

            vert_arr[i].color = SColor{ float(rand()&0xFF)/255.0f,
                                        float(rand()&0xFF)/255.0f,
                                        float(rand()&0xFF)/255.0f };
        }

        rasterizer.fill_face(vert_arr);
    } 

    buf.render(rasterizer.frag_vec());
    scr.write(buf.data(), buf.byte_size());

    return 0;
}

//} //namespace sgl 

#endif //SGL_CRASTERIZER_H
