#ifndef SGL_CPARALLELRASTERIZER_H
#define SGL_CPARALLELRASTERIZER_H

#include "parallel/CThreadPool.h"
#include "memory/CIntrinsicAllocator.h"
#include "CRasterizer.h"

//namespace sgl {

//TODO: to implement thread pool sync() function
class CParallelRasterizer
{
public:
    static const size_t THREAD_CNT = 8;

    //using vert_shader_t = SVertexShader;
    //using frag_shader_t = SFragmentShader;

    CParallelRasterizer();

    void sync() const
    {
        thread_pool_.sync();
    }

    void rast_line(const SVertex& beg_v, 
                   const SVertex& end_v);

    void rast_face(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v);

    void fill_face(const SVertex v_arr[3]);

private:
    CThreadPool thread_pool_;
    CRasterizer rasterizer_arr_[THREAD_CNT];
};

void CParallelRasterizer::rast_line(const SVertex& beg_v, 
                                    const SVertex& end_v)
{
    auto task = [this](size_t i)
    {
        rasterizer_arr_[i].rast_line(beg_v, end_v);
    };

    thread_pool_.push_task(std::move(task));
}

void CParallelRasterizer::rast_face(const SVertex& beg_v, 
                                    const SVertex& mid_v, 
                                    const SVertex& end_v)
{
    auto task = [this](size_t i)
    {
        rasterizer_arr_[i].rast_face(beg_v, mid_v, end_v);
    };

    thread_pool_.push_task(std::move(task));
}

void CParallelRasterizer::fill_face(const SVertex v_arr[3])
{
    auto task = [this](size_t i)
    {
        rasterizer_arr_[i].fill_face(v_arr);
    };

    thread_pool_.push_task(std::move(task));
}

//} //namespace sgl

#endif //SGL_CPARALLELRASTERIZER_H
