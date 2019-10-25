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
    CRasterizer(float max_x_set, float max_y_set);

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

    void fill_face(const SVertex v_arr[3]);

    void fill_half(const SFragmentExt& beg_f, const SFragmentExt& end_f,
                   const SFragmentExt& top_f);

    void fill_xseq(const SFragmentExt& beg_f, const SFragmentExt& end_f);

private:
    std::vector<SFragment> frag_vec_;

    float max_x_;
    float max_y_;
};

CRasterizer::CRasterizer(float max_x_set, float max_y_set):
    frag_vec_(),
    max_x_(max_x_set),
    max_y_(max_y_set)
{}

CRasterizer::CRasterizer(CRasterizer&& move_rast):
    frag_vec_(std::move(move_rast.frag_vec_)),
    max_x_   (std::move(move_rast.max_x_)),
    max_y_   (std::move(move_rast.max_y_))
{}

CRasterizer& CRasterizer::operator = (CRasterizer&& move_rast)
{
    std::swap(frag_vec_, move_rast.frag_vec_);
    std::swap(max_x_,    move_rast.max_x_);
    std::swap(max_y_,    move_rast.max_y_);

    return *this;
}

CRasterizer::~CRasterizer()
{
    max_x_ = max_y_ = 0;

    frag_vec_.clear();
}

void CRasterizer::rast_line(const SVertex& beg_v, 
                            const SVertex& end_v)
{
    SFragmentExt beg_f = ::to_fragment(beg_v);
    SFragmentExt end_f = ::to_fragment(end_v);

    size_t step_cnt = std::max(size_t(fabs(end_f.frag.point.x - 
                                           beg_f.frag.point.x)),
                               size_t(fabs(end_f.frag.point.y - 
                                           beg_f.frag.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        frag_vec_.push_back(::interpolate(beg_f, end_f, ratio).frag);
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
    SFragmentExt beg_f = ::to_fragment(v_arr[0]);
    SFragmentExt mid_f = ::to_fragment(v_arr[1]);
    SFragmentExt end_f = ::to_fragment(v_arr[2]);

    if (beg_f.frag.point.y < mid_f.frag.point.y) std::swap(beg_f, mid_f);
    if (mid_f.frag.point.y < end_f.frag.point.y) std::swap(mid_f, end_f);
    if (beg_f.frag.point.y < mid_f.frag.point.y) std::swap(beg_f, mid_f);

    if (fabs(beg_f.frag.point.y - end_f.frag.point.y) < FLT_EPSILON)
    {
        fill_xseq(beg_f, mid_f);
        fill_xseq(end_f, mid_f);
    }
    else
    {
        float ratio = (beg_f.frag.point.y - mid_f.frag.point.y) / 
                      (beg_f.frag.point.y - end_f.frag.point.y);

        SFragmentExt div_f = ::interpolate(beg_f, end_f, ratio);

        fill_half(div_f, mid_f, beg_f);
        fill_half(div_f, mid_f, end_f);
        fill_xseq(div_f, mid_f);
    }
}

void CRasterizer::fill_half(const SFragmentExt& beg_f, 
                            const SFragmentExt& end_f,
                            const SFragmentExt& top_f)
{ //TODO: check beg & end Y coordinate equality requirement
    size_t step_cnt = size_t(roundf(fabs(top_f.frag.point.y - 
                                         beg_f.frag.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        
        fill_xseq(::interpolate(top_f, beg_f, ratio), 
                  ::interpolate(top_f, end_f, ratio));
    }
}

void CRasterizer::fill_xseq(const SFragmentExt& beg_f, 
                            const SFragmentExt& end_f)
{
    size_t step_cnt = size_t(roundf(fabs(beg_f.frag.point.x - 
                                         end_f.frag.point.x)));

    for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        frag_vec_.push_back(::interpolate(beg_f, end_f, ratio).frag);
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

            vert_arr[i].color = SColor(float(rand()&0xFF)/255.0f,
                                       float(rand()&0xFF)/255.0f,
                                       float(rand()&0xFF)/255.0f);
        }

        rasterizer.fill_face(vert_arr);
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
