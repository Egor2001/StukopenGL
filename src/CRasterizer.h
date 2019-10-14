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

    void fill_face(const SVertex[3] v_arr);

    void fill_half(const SVertex& beg_v, const SVertex& end_v,
                   const SVertex& top_v);

    void fill_xseq(const SVertex& cur_beg_v, const SVertex& cur_end_v);

    //TODO: make it static
    SVertex interpolate(const SVertex& beg_v, const SVertex& end_v, 
                        float ratio);

    //TODO: make it static
    SFragment get_fragment(const SVertex& vertex);

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
        //step_cnt to-float conversion should be optimized
        float ratio = (float(cur_step) / float(step_cnt));
        SVertex cur_vertex = interpolate(beg_v, end_v, ratio);

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

void CRasterizer::fill_face(const SVertex[3] v_arr)
{
    size_t idx_perm[3] = {0, 1, 2};

#define CMP_Y_AND_SWAP(MAX_IDX, MIN_IDX) \
    if (v_arr[idx_perm[MAX_IDX]].point.y < v_arr[idx_perm[MIN_IDX]].point.y) \
        std::swap(idx_perm[MAX_IDX], idx_perm[MIN_IDX]);

    CMP_Y_AND_SWAP(0, 1)
    CMP_Y_AND_SWAP(1, 2)
    CMP_Y_AND_SWAP(0, 1)

#undef CMP_AND_SWAP

    float ratio = (v_arr[idx_perm[0]].y - v_arr[idx_perm[1]].y) /
                  (v_arr[idx_perm[0]].y - v_arr[idx_perm[2]].y);

    SVertex mid_v = interpolate(SVertex[0], SVertex[1], ratio);

    fill_half(mid_v, v_arr[1], v_arr[0]);
    fill_half(mid_v, v_arr[1], v_arr[2]);
}

void CRasterizer::fill_half(const SVertex& beg_v, const SVertex& end_v,
                            const SVertex& top_v)
{
    //TODO: check beg & end X coordinate equality requirement
    size_t step_cnt = size_t(fabs(top_v.point.y - beg_v.point.y));

    for (cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));

        SVertex cur_beg_v = interpolate(top_v, beg_v, ratio);
        SVertex cur_end_v = interpolate(top_v, end_v, ratio);
        
        fill_xseq(cur_beg_v, cur_end_v);
    }
}

void CRasterizer::fill_xseq(const SVertex& cur_beg_v, 
                            const SVertex& cur_end_v)
{
}

SVertex CRasterizer::interpolate(const SVertex& beg_v, const SVertex& end_v, 
                                 float ratio)
{
    SVertex result = {};

    result.point = (beg_v.point * (1.0f - ratio) + 
                    end_v.point *         ratio);

    result.normal = (beg_v.normal * (1.0f - ratio) + 
                     end_v.normal *         ratio);

    result.color = ::interpolate(beg_v.color, end_v.color, ratio);

    return result;
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
