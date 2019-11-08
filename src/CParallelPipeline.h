#ifndef SGL_CPARALLELPIPELINE_H
#define SGL_CPARALLELPIPELINE_H

#include "CObject.h"
#include "math/SColor.h"
#include "SCamera.h"
#include "SLight.h"
#include "SVertexShader.h"
#include "SFragmentShader.h"
#include "CPerspective.h"
#include "CFillRasterizer.h"
#include "CBuffer.h"
#include "CScreen.h"
#include "SScene.h"
#include "parallel/CThreadPool.h"
#include "parallel/CLatch.h"

//namespace sgl {

template<template<typename> typename R>
class CParallelPipeline
{
public:
    static const size_t THREAD_CNT = 8;

    using TVertexContainer = CIntrinsicVector<SVertex>;
    using TFragmentContainer = CIntrinsicVector<SFragment>;
    using TRasterizer = R<TFragmentContainer>;

    template<typename... Types>
    explicit CParallelPipeline(Types&&... rasterizer_args);

    ~CParallelPipeline();

    template<typename VS, typename FS>
    void render_scene(const CObject&, 
                      const VS& vert_shader, const FS& frag_shader);

    void flush_screen(CScreen&);
    void clear_buffer();

private:
//    std::mutex                   buffer_mutex_;
    std::shared_ptr<CThreadPool> thread_pool_;

    TVertexContainer vert_buf_;
    std::vector<TFragmentContainer> frag_buf_vec_;

    TRasterizer rasterizer_;
    CBuffer     buffer_;
};

template<template<typename> typename R>
template<typename... Types>
CParallelPipeline<R>::
CParallelPipeline(Types&&... rasterizer_args):
//    buffer_mutex_(),
    thread_pool_(std::make_shared<CThreadPool>(THREAD_CNT)),
    vert_buf_(),
    frag_buf_vec_(THREAD_CNT),
    rasterizer_(std::forward<Types>(rasterizer_args)...),
    buffer_()
{}

template<template<typename> typename R>
CParallelPipeline<R>::
~CParallelPipeline()
{}

template<template<typename> typename R>
template<typename VS, typename FS>
void 
CParallelPipeline<R>::
render_scene(const CObject& object, 
             const VS& vert_shader, const FS& frag_shader)
{
    for (const auto& vertex: object.vertex_buf())
        vert_buf_.push_back(vertex);

    for (auto& vert : vert_buf_)
        vert_shader(vert);

    CLatch latch(THREAD_CNT+1);
    auto render_task = [this, &vert_shader, &frag_shader, 
                        &object, &latch](size_t thread_idx)
    {
        size_t index_buf_size = object.index_buf().size();

        auto cur_beg = object.index_buf().cbegin() + 
            ((thread_idx + 0)*index_buf_size)/THREAD_CNT;
        auto cur_end = object.index_buf().cbegin() + 
            ((thread_idx + 1)*index_buf_size)/THREAD_CNT;

        for (auto it = cur_beg; it != cur_end; ++it)
        {
            rasterizer_.rasterize(vert_buf_[it->arr[0]],
                                  vert_buf_[it->arr[1]],
                                  vert_buf_[it->arr[2]], 
                                  frag_buf_vec_[thread_idx]);
        }

        for (auto& frag : frag_buf_vec_[thread_idx])
            frag_shader(frag, vert_buf_);

        latch.count_down_and_wait();
    };

    for (size_t idx = 0; idx < THREAD_CNT; ++idx)
        thread_pool_->push_task(render_task);

    latch.count_down_and_wait();

    for (size_t idx = 0; idx < THREAD_CNT; ++idx)
    {
        buffer_.render(frag_buf_vec_[idx]);
        frag_buf_vec_[idx].clear();
    }

    vert_buf_.clear();
}

//TODO: use smart pointer instead of non-const reference
template<template<typename> typename R>
void 
CParallelPipeline<R>::
flush_screen(CScreen& screen)
{
//    std::lock_guard<std::mutex> lock(buffer_mutex_);

    screen.write(buffer_.data(), buffer_.byte_size());
}

//TODO: to vary depth value
template<template<typename> typename R>
void 
CParallelPipeline<R>::
clear_buffer()
{
//    std::lock_guard<std::mutex> lock(buffer_mutex_);

    buffer_.clear(CBuffer::SBufColor(), FLT_MAX);
}

//} //namespace sgl

#endif //SGL_CPARALLELPIPELINE_H
