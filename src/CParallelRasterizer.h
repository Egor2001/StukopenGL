#ifndef SGL_CPARALLELRASTERIZER_H
#define SGL_CPARALLELRASTERIZER_H

#include "parallel/CThreadPool.h"
#include "parallel/CLatch.h"
#include "memory/CIntrinsicAllocator.h"
#include "CRasterizer.h"

//namespace sgl {

class CParallelRasterizer
{
public:
    static const size_t THREAD_CNT = 8;

    //using vert_shader_t = SVertexShader;
    //using frag_shader_t = SFragmentShader;

    CParallelRasterizer(float max_x_set, float max_y_set);

    CParallelRasterizer             (const CParallelRasterizer&) = delete;
    CParallelRasterizer& operator = (const CParallelRasterizer&) = delete;

    CParallelRasterizer             (CParallelRasterizer&&);
    CParallelRasterizer& operator = (CParallelRasterizer&&);

    ~CParallelRasterizer();

    //TODO: to replace vector with something more flexible 
    CIntrinsicVector<SFragment> frag_vec() const;

    void clear();

    void rast_line(const SVertex& beg_v, 
                   const SVertex& end_v);

    void rast_face(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v);

    void fill_face(const SVertex v_arr[3]);

private:
    //mutable to make parallel rasterizer meet rasterizer interface
    mutable CThreadPool thread_pool_;

    CRasterizer rasterizer_arr_[THREAD_CNT];
};

CIntrinsicVector<SFragment> CParallelRasterizer::frag_vec() const;
{
    CIntrinsicVector result;

    //TODO: to use latch from concurrency TS
    CLatch latch{ THREAD_CNT };
    for (size_t i = 0; i < THREAD_CNT; ++i)
    {
        thread_pool_.push_task([this, &latch](size_t thread_idx)
        {
            //rasterizer_arr_[thread_idx_].apply_frag_shader(frag_shader);
            latch.count_down_and_wait();
        });
    }
    latch.wait();

    //TODO: to replace copying with something not so expensive 
    for (size_t i = 0; i < THREAD_CNT; ++i)
        result.insert(result.end(), 
                      rasterizer_arr_[i].frag_vec().begin(),
                      rasterizer_arr_[i].frag_vec().end());

    return result;
}

CParallelRasterizer::void clear()
{
    //TODO: to use latch from concurrency TS
    CLatch latch{ THREAD_CNT };
    for (size_t i = 0; i < THREAD_CNT; ++i)
    {
        thread_pool_.push_task([this, &latch](size_t thread_idx)
        {
            rasterizer_arr_[thread_idx_].clear();
            latch.count_down_and_wait();
        });
    }
    latch.wait();
}

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
