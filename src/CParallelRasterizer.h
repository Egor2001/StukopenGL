#ifndef SGL_CPARALLELRASTERIZER_H
#define SGL_CPARALLELRASTERIZER_H

//#include <shared_ptr>

#include "parallel/CThreadPool.h"
#include "parallel/CLatch.h"
#include "memory/CIntrinsicAllocator.h"

//namespace sgl {

template<typename R>
class CParallelRasterizer
{
public:
    using TRasterizer = R;

    CParallelRasterizer(float max_x_set, float max_y_set,
                        const std::shared_ptr<CThreadPool>& thread_pool_set);

    CParallelRasterizer             (const CParallelRasterizer&) = delete;
    CParallelRasterizer& operator = (const CParallelRasterizer&) = delete;

    CParallelRasterizer             (CParallelRasterizer&&);
    CParallelRasterizer& operator = (CParallelRasterizer&&);

    ~CParallelRasterizer();

    //TODO: to create template (! yet another one) parameter for thread count 
    template<typename TParallelContainer>
    void rasterize(const SVertex& beg_v, 
                   const SVertex& mid_v, 
                   const SVertex& end_v,
                   TParallelContainer& frag_container);

private:
    std::shared_ptr<CThreadPool> thread_pool_;
    std::vector<TRasterizer>     rasterizer_vec_;
};

template<typename R>
CParallelRasterizer<R>::
CParallelRasterizer(float max_x_set, float max_y_set, 
                    const std::shared_ptr<CThreadPool>& thread_pool_set):
    thread_pool_(thread_pool_set),
    rasterizer_vec_()
{
    size_t thread_cnt = thread_pool_.thread_cnt();
    for (size_t i = 0; i < thread_cnt; ++i)
        rasterizer_vec_.emplace_back(max_x_set, max_y_set);
}

template<typename R>
CParallelRasterizer<R>::
CParallelRasterizer(CParallelRasterizer<R>&& move_rasterizer):
    thread_pool_   (std::move(move_rasterizer.thread_pool_)),
    rasterizer_vec_(std::move(move_rasterizer.rasterizer_vec_))
{}

template<typename R>
CParallelRasterizer<R>& 
CParallelRasterizer<R>::
operator = (CParallelRasterizer<R>&& move_rasterizer)
{
    std::swap(thread_pool_,    move_rasterizer.thread_pool_);
    std::swap(rasterizer_vec_, move_rasterizer.rasterizer_vec_);

    return *this;
}

CParallelRasterizer::~CParallelRasterizer()
{}

template<typename TParallelContainer>
template<typename R>
void 
CParallelRasterizer<R>::
rasterize(const SVertex& beg_v, 
          const SVertex& mid_v, 
          const SVertex& end_v,
          TParallelContainer& frag_container)
{
    auto task = [this, &beg_v, &mid_v, &end_v,
                 &frag_container](size_t thread_idx)
    {
        auto& async_container_ref = 
            frag_container.async_container(thread_idx);
        rasterizer_vec_[i].fill_face(beg_v, mid_v, end_v,
                                     async_container_ref);
    };

    thread_pool_.push_task(std::move(task));
}

//} //namespace sgl

#endif //SGL_CPARALLELRASTERIZER_H
