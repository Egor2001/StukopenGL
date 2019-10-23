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

    void fill_half(const SVertex& beg_v, const SVertex& end_v,
                   const SVertex& top_v);

    void fill_xseq(const SVertex& beg_v, const SVertex& end_v);

    //TODO: make it static
    SFragment get_fragment(const SVertex& vertex);

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
    size_t step_cnt = std::max(size_t(fabs(end_v.point.x - beg_v.point.x)),
                               size_t(fabs(end_v.point.y - beg_v.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        //step_cnt to-float conversion should be optimized
        float ratio = (float(cur_step) / float(step_cnt));
        SVertex cur_vertex = ::interpolate(beg_v, end_v, ratio);

        frag_vec_.push_back(get_fragment(cur_vertex));
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
    SVertex beg_v = v_arr[0]; 
    SVertex mid_v = v_arr[1]; 
    SVertex end_v = v_arr[2]; 

    if (beg_v.point.y < mid_v.point.y) std::swap(beg_v, mid_v);
    if (mid_v.point.y < end_v.point.y) std::swap(mid_v, end_v);
    if (beg_v.point.y < mid_v.point.y) std::swap(beg_v, mid_v);

    if (fabs(beg_v.point.y - end_v.point.y) < FLT_EPSILON)
    {
        fill_xseq(beg_v, mid_v);
        fill_xseq(end_v, mid_v);
    }
    else
    {
        float ratio = (beg_v.point.y - mid_v.point.y) / 
                      (beg_v.point.y - end_v.point.y);

        SVertex div_v = ::interpolate(beg_v, end_v, ratio);

        fill_half(div_v, mid_v, beg_v);
        fill_half(div_v, mid_v, end_v);
        fill_xseq(div_v, mid_v);
    }
}

void CRasterizer::fill_half(const SVertex& beg_v, const SVertex& end_v,
                            const SVertex& top_v)
{ //TODO: check beg & end Y coordinate equality requirement
    size_t step_cnt = size_t(roundf(fabs(top_v.point.y - beg_v.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));

        SVertex cur_beg_v = ::interpolate(top_v, beg_v, ratio);
        SVertex cur_end_v = ::interpolate(top_v, end_v, ratio);
        
        fill_xseq(cur_beg_v, cur_end_v);
    }
}

void CRasterizer::fill_xseq(const SVertex& beg_v, 
                            const SVertex& end_v)
{
/*
    size_t step_cnt = size_t(roundf(fabs(beg_v.point.x - 
                                         end_v.point.x)));

    for (size_t cur_step = 0; cur_step <= step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        SVertex cur_vertex = ::interpolate(beg_v, end_v, ratio);

        frag_vec_.push_back(get_fragment(cur_vertex));
    }
*/
    size_t seq_y = size_t(roundf(beg_v.point.x));
    size_t beg_x = size_t(fmax(0.0f, fmin(roundf(beg_v.point.x), max_x_)));
    size_t end_x = size_t(fmax(0.0f, fmin(roundf(end_v.point.x), max_x_)));

    SFragment::SFragColor beg_frag_color =  

    if (beg_x <= end_x && 
        beg_v.point.y > 0.0f && 
        beg_v.point.y < max_y_)
    {
        size_t delta_x = end_x - beg_x;

        float cur_ratio = 0.0f;
        for (size_t x = 0; x <= delta_x; ++x)
        {
            cur_ratio = float(x)/float(delta_x);
            
        }
    }
}

SFragment CRasterizer::get_fragment(const SVertex& vertex)
{
    SFragment result = {};

    result.x = size_t(vertex.point.x);
    result.y = size_t(vertex.point.y);

    result.depth = vertex.point.z;
    result.color = { uint8_t(255.0f*vertex.color.r),
                     uint8_t(255.0f*vertex.color.g),
                     uint8_t(255.0f*vertex.color.b),
                     uint8_t(255.0f*vertex.color.a) };

    return result;
}

int test_CRasterizer()
{
    srand(time(NULL));
    
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();
    CRasterizer rasterizer = CRasterizer(float(CBuffer::DIM_W),
                                         float(CBuffer::DIM_H));

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
