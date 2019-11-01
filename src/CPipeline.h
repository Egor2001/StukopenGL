#ifndef SGL_CPIPELINE_H
#define SGL_CPIPELINE_H

#include "CObject.h"
#include "SColor.h"
#include "CCamera.h"
#include "CLight.h"
#include "CVertexShader.h"
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
    CCamera    camera;
    CLight     light;
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
    SMatrixExt proj_mtx = buffer_.get_matrix()*projection_.get_matrix();

    CVertexShader vert_shader = CVertexShader(scene.camera.get_matrix()*
                                              scene.matrix); 

    SVertex face[3] = {};
    for (auto& index : scene.object.index_buf())
    {
        //TODO: to fix object class to set color in object
        for (size_t i = 0; i < 3; ++i)
        {
            face[i] = scene.object.vertex_buf()[index.arr[i]];
            face[i].color = SColor{ 0.5f, 0.5f, 0.5f };
            face[i] = scene.light.apply(vert_shader.apply(face[i]));
            face[i].point = proj_mtx*face[i].point;
        }

        rasterizer_.fill_face(face);
    }

    printf("fragments vector size: %lu\n", rasterizer_.frag_vec().size());
//    for (auto& frag : rasterizer_.frag_vec())
//        printf("point: x=%.3f y=%.3f z=%.3f; "
//               "color: r=%.3f g=%.3f b=%.3f\n", 
//                frag.point.x, frag.point.y, frag.point.z,
//                frag.color.r, frag.color.g, frag.color.b);

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
