#ifndef SGL_CPIPELINE_H
#define SGL_CPIPELINE_H

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

//namespace sgl {

template<typename R, 
         typename VS = SVertexShader, 
         typename FS = SFragmentShader>
class CPipeline
{
public:
    using TRasterizer = R;
    using TFragContainer = typename TRasterizer::TFragmentContainer;

    using TVertShader = VS; 
    using TFragShader = FS; 

    template<typename... Types>
    explicit CPipeline(Types&&... rasterizer_args);

    ~CPipeline();

    void render_scene(const SScene&, const CObject&);
    void flush_screen(CScreen&);
    void clear_buffer();

private:
    CIntrinsicVector<SVertex>   vert_buf_;
    CIntrinsicVector<SFragment> frag_buf_;

    TVertShader vert_shader_;
    TFragShader frag_shader_;

    TRasterizer rasterizer_;
    CBuffer     buffer_;
};

template<typename R, typename VS, typename FS>
template<typename... Types>
CPipeline<R, VS, FS>::
CPipeline(Types&&... rasterizer_args):
    vert_buf_(),
    frag_buf_(),
    vert_shader_(),
    frag_shader_(),
    rasterizer_(std::forward<Types>(rasterizer_args)...),
    buffer_(/*rasterizer_.max_x(), rasterizer_.max_y()*/)
{}

template<typename R, typename VS, typename FS>
CPipeline<R, VS, FS>::
~CPipeline()
{
    buffer_.clear(CBuffer::SBufColor(), 1.0f);
    frag_buf_.clear();
    vert_buf_.clear();
}

template<typename R, typename VS, typename FS>
void 
CPipeline<R, VS, FS>::
render_scene(const SScene& scene, const CObject& object)
{
    for (const auto& vert : object.vertex_buf())
        vert_buf_.push_back(vert);

    vert_shader_.init(scene, 
                      float(CBuffer::DIM_W), 
                      float(CBuffer::DIM_H));
    for (auto& vert : vert_buf_)
        vert_shader_.apply(vert);

    for (auto& index : object.index_buf())
        rasterizer_.rasterize(vert_buf_[index.arr[0]], 
                              vert_buf_[index.arr[1]],
                              vert_buf_[index.arr[2]], frag_buf_);

    frag_shader_.init(scene, 
                      float(CBuffer::DIM_W), 
                      float(CBuffer::DIM_H));
    for (auto& frag : frag_buf_)
        frag_shader_.apply(frag, vert_buf_);

    buffer_.render(frag_buf_);
    //must be fast (constant time) for trivial types
    //TODO: consider to move to clear() function

    frag_buf_.clear();
    vert_buf_.clear();
}

//TODO: use smart pointer instead of non-const reference
template<typename R, typename VS, typename FS>
void 
CPipeline<R, VS, FS>::
flush_screen(CScreen& screen)
{
    screen.write(buffer_.data(), buffer_.byte_size());
}

//TODO: to vary depth value
template<typename R, typename VS, typename FS>
void 
CPipeline<R, VS, FS>::
clear_buffer()
{
    buffer_.clear(CBuffer::SBufColor(), FLT_MAX);
}

//} //namespace sgl

#endif //SGL_CPIPELINE_H
