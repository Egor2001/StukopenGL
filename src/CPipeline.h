#ifndef SGL_CPIPELINE_H
#define SGL_CPIPELINE_H

#include "CObject.h"
#include "math/SColor.h"
#include "SCamera.h"
#include "SLight.h"
#include "SVertexShader.h"
#include "SFragmentShader.h"
#include "SPerspective.h"
#include "CFillRasterizer.h"
#include "CBuffer.h"
#include "CScreen.h"
#include "SScene.h"
#include "SIndex.h"

//namespace sgl {

template<template<typename> typename R>
class CPipeline
{
public:
    using TVertexContainer = CIntrinsicVector<SVertex>;
    using TFragmentContainer = CIntrinsicVector<SFragment>;
    using TRasterizer = R<TFragmentContainer>;

    template<typename... Types>
    explicit CPipeline(Types&&... rasterizer_args);

    ~CPipeline();

    template<typename VS, typename FS, typename CF>
    void render_scene(const std::vector<SVertex>&, 
                      const std::vector<SIndex>&, 
                      const VS&, const FS&, const CF&);
    void flush_screen(CScreen&);
    void clear_buffer();

private:
    TVertexContainer   vert_buf_;
    TFragmentContainer frag_buf_;

    TRasterizer rasterizer_;
    CBuffer     buffer_;
};

template<template<typename> typename R>
template<typename... Types>
CPipeline<R>::
CPipeline(Types&&... rasterizer_args):
    vert_buf_(),
    frag_buf_(),
    rasterizer_(std::forward<Types>(rasterizer_args)...),
    buffer_(/*rasterizer_.max_x(), rasterizer_.max_y()*/)
{}

template<template<typename> typename R>
CPipeline<R>::
~CPipeline()
{
    buffer_.clear(CBuffer::SBufColor(), 1.0f);
    frag_buf_.clear();
    vert_buf_.clear();
}

template<template<typename> typename R>
template<typename VS, typename FS, typename CF>
void 
CPipeline<R>::
render_scene(const std::vector<SVertex>& vertex_buf, 
             const std::vector<SIndex>& index_buf, 
             const VS& vert_shader, const FS& frag_shader,
             const CF& cull_face)
{
    for (const auto& vertex: vertex_buf)
        vert_buf_.push_back(vertex);

    for (auto& vert : vert_buf_)
        vert_shader(vert);

    for (auto& index : index_buf)
    {
        if (cull_face(vert_buf_[index.arr[0]],
                      vert_buf_[index.arr[1]],
                      vert_buf_[index.arr[2]]))
        {
            continue;
        }

        rasterizer_.rasterize(vert_buf_[index.arr[0]],
                              vert_buf_[index.arr[1]],
                              vert_buf_[index.arr[2]], 
                              frag_buf_);
    }

    for (auto& frag : frag_buf_)
        frag_shader(frag, vert_buf_);

    buffer_.render(frag_buf_);

    frag_buf_.clear();
    vert_buf_.clear();
}

//TODO: use smart pointer instead of non-const reference
template<template<typename> typename R>
void 
CPipeline<R>::
flush_screen(CScreen& screen)
{
    screen.write(buffer_.data(), buffer_.byte_size());
}

//TODO: to vary depth value
template<template<typename> typename R>
void 
CPipeline<R>::
clear_buffer()
{
    buffer_.clear(CBuffer::SBufColor(), FLT_MAX);
}

//} //namespace sgl

#endif //SGL_CPIPELINE_H
