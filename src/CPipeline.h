#ifndef SGL_CPIPELINE_H
#define SGL_CPIPELINE_H

#include "CObject.h"
#include "math/SColor.h"
#include "SCamera.h"
#include "SLight.h"
#include "SVertexShader.h"
#include "SFragmentShader.h"
#include "CPerspective.h"
#include "CRasterizer.h"
#include "CBuffer.h"
#include "CScreen.h"

//namespace sgl {

//TODO: move out into separate file
//TODO: to add support for multiple objects 
//TODO: to add support for multiple lights
struct SScene
{
    CObject    object;
    SMatrixExt matrix;
    SCamera    camera;
    SLight     light;
};

class CPipeline
{
public:
    using rast_t = CRasterizer;
    using proj_t = CPerspective;

    CPipeline(rast_t&& rasterizer_set,
              proj_t&& projection_set);

    ~CPipeline();

    void render_scene(const SScene& scene);
    void flush_screen(CScreen& screen);
    void clear_buffer();

private:
    CBuffer buffer_;
    rast_t rasterizer_;
    proj_t projection_;
};

CPipeline::CPipeline(rast_t&& rasterizer_set,
                     proj_t&& projection_set):
    buffer_(),
    rasterizer_(std::move(rasterizer_set)),
    projection_(std::move(projection_set))
{}

CPipeline::~CPipeline()
{
    buffer_.clear(CBuffer::SBufColor(), 1.0f);
    rasterizer_.clear();
}

void CPipeline::render_scene(const SScene& scene)
{
    SVertexShader vert_shader = SVertexShader
    {
        .modelview_mtx = scene.camera.get_matrix()*
                         scene.matrix,

        .projection_mtx = buffer_.get_matrix()*
                          projection_.get_matrix()
    }; 

    SFragmentShader frag_shader = SFragmentShader{};
    
    SLight light = scene.light;
    light.point = light.point - scene.camera.pos;

    SVertex face[3] = {};
    for (auto& index : scene.object.index_buf())
    {
        for (size_t i = 0; i < 3; ++i)
        {
            face[i] = vert_shader.apply(scene.object.vertex_buf()[index.arr[i]],
                                        light, SVector{});
        }

        rasterizer_.fill_face(face);
    }

    buffer_.render(rasterizer_.frag_vec());
    rasterizer_.clear();
}

//TODO: use smart pointer instead of non-const reference
void CPipeline::flush_screen(CScreen& screen)
{
    screen.write(buffer_.data(), buffer_.byte_size());
}

//TODO: to vary depth value
void CPipeline::clear_buffer()
{
    buffer_.clear(CBuffer::SBufColor(), FLT_MAX);
}

//} //namespace sgl

#endif //SGL_CPIPELINE_H
