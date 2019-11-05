#ifndef SGL_CLINERASTERIZER_H
#define SGL_CLINERASTERIZER_H

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

class CLineRasterizer
{
public:
    CLineRasterizer(float max_x_set, float max_y_set):
        max_x_(max_x_set),
        max_y_(max_y_set)
    {}

    float max_x() const noexcept { return max_x_; }
    float max_y() const noexcept { return max_y_; }

    template<typename TContainer>
    void rasterize(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v, 
                   TContainer& frag_container);

private:
    template<typename TContainer>
    void rast_line(const SVertex& beg_v, 
                   const SVertex& end_v, 
                   TContainer& frag_container);

    float max_x_;
    float max_y_;
};

template<typename TContainer>
void CLineRasterizer::rasterize(const SVertex& beg_v, 
                                const SVertex& mid_v, 
                                const SVertex& end_v,
                                TContainer& frag_container)
{
    rast_line(beg_v, mid_v, frag_container);
    rast_line(mid_v, end_v, frag_container);
    rast_line(end_v, beg_v, frag_container);
}

template<typename TContainer>
void CRasterizer::rast_line(const SVertex& beg_v, 
                            const SVertex& end_v,
                            TContainer& frag_container)
{
    SFragment beg_f = ::to_fragment(beg_v);
    SFragment end_f = ::to_fragment(end_v);

    size_t step_cnt = std::max(size_t(fabs(end_f.point.x - beg_f.point.x)),
                               size_t(fabs(end_f.point.y - beg_f.point.y)));

    for (size_t cur_step = 0; cur_step < step_cnt; ++cur_step)
    {
        float ratio = (float(cur_step) / float(step_cnt));
        frag_container.push_back(::interpolate(beg_f, end_f, ratio));
    }
}

int test_CLineRasterizer()
{
    srand(time(NULL));
    
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();
    CLineRasterizer rasterizer 
        = CLineRasterizer(float(CBuffer::DIM_W),
                          float(CBuffer::DIM_H));

    CIntrinsicVector<SFragment> frag_vec;

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

        rasterizer.rasterize(vert_arr[0], vert_arr[1], vert_arr[2],
                             frag_vec);
    } 

    buf.render(frag_vec);
    scr.write(buf.data(), buf.byte_size());

    return 0;
}

//} //namespace sgl 

#endif //SGL_CLINERASTERIZER_H
