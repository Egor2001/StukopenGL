//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <chrono>
#include <ctime>

//double including all tested files to check collisions

#include "src/math/SVectorExt.h"
#include "src/math/SMatrixExt.h"
#include "src/math/SColorExt.h"
#include "src/CFillRasterizer.h"
#include "src/CLineRasterizer.h"
#include "src/CScreen.h"
#include "src/CBuffer.h"
#include "src/CObject.h"
#include "src/SCamera.h"
#include "src/SVertexShader.h"
#include "src/SFragmentShader.h"
#include "src/SLight.h"
#include "src/SPerspective.h"
#include "src/CPipeline.h"
#include "src/CParallelPipeline.h"
#include "src/CTexture.h"
#include "src/CCullFace.h"

#define SGL_INIT_RESOURCE_FILE(SGL_ARG_NAME, SGL_ARG_PATH) \
    FILE* SGL_ARG_NAME = fopen(SGL_ARG_PATH, "r+"); \
    if (!SGL_ARG_NAME) \
    { \
        perror("error opening resource file " #SGL_ARG_NAME ": "); \
        return EXIT_FAILURE; \
    }

#define SGL_CLOSE_RESOURCE_FILE(SGL_ARG_NAME) \
    if (!SGL_ARG_NAME) \
    { \
        fclose(SGL_ARG_NAME); \
        SGL_ARG_NAME = nullptr; \
    }

int main(int argc, char* argv[])
{
    if (argc != 5)
    {
        printf("Usage: %s OBJFILE TEXFILE \n", argv[0]);
        return EXIT_FAILURE;
    }

    SGL_INIT_RESOURCE_FILE(obj_file_plane, argv[1]);
    SGL_INIT_RESOURCE_FILE(tex_file_plane, argv[2]);
    SGL_INIT_RESOURCE_FILE(obj_file_sky,   argv[3]);
    SGL_INIT_RESOURCE_FILE(tex_file_sky,   argv[4]);

    SScene scene = 
    {
        .dim_x = float(CBuffer::DIM_W),
        .dim_y = float(CBuffer::DIM_H),

        .camera = SCamera
        {
            .pos = SVector{ 0.0f, 0.0f, 10.0f },
            .dir = SVector{ 0.0f, 0.0f, 1.0f },
            .up  = SVector{ 0.0f, 1.0f, 0.0f }
        },

        .light = SLight
        { 
            .point = SVector{ -2.5f, -1.0f, 0.0f },
            .color = SColor{ 1.0f, 1.0f, 1.0f },
            .phong_ads = SVector{ 0.125f, 0.250f, 0.625f },
            .phong_pow = 16.0f
        },

        .projection = SPerspective
        {
            .near = 1.0f, 
            .far = 2.0f
        }
    };

    float side = fmin(scene.dim_x, scene.dim_y);
    
    float xx = 0.5f*side;  
    float yy = -0.5f*side; 

    float xw = 0.5f*scene.dim_x; 
    float yw = 0.5f*scene.dim_y; 

    SMatrixExt view_mtx = SMatrixExt(SVectorExt{ xx,   0.0f, 0.0f, xw },
                                     SVectorExt{ 0.0f, yy,   0.0f, yw },
                                     SVectorExt{ 0.0f, 0.0f, 1.0f, 0.0f});

    CObject object_plane;
    CTexture texture_plane(tex_file_plane);
    object_plane.parse_from(obj_file_plane, SColor{ 1.0f, 1.0f, 1.0f });

    CObject object_sky;
    CTexture texture_sky(tex_file_sky);
    object_sky.parse_from(obj_file_sky, SColor{ 1.0f, 1.0f, 1.0f });

    auto cull_face_plane = CCullFace<SGL_CULL_BACK>();
    auto cull_face_sky   = CCullFace<SGL_CULL_FRONT>();

    auto frag_shader_plane = [&texture_plane]
    (SFragment& frag, const auto& vert_buf)
    {
        frag.color *= 
            texture_plane.get_color(frag.tex_u, 1.0f - frag.tex_v);
    };

    auto frag_shader_sky = [&texture_sky]
    (SFragment& frag, const auto& vert_buf)
    {
        frag.color = 
            texture_sky.get_color(frag.tex_u, 1.0f - frag.tex_v);
    };

    auto pipeline = 
        CPipeline<CFillRasterizer>(scene.dim_x, scene.dim_y); 

    CScreen screen = CScreen();

    auto beg_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> seconds_elapsed;

    double sum_fps = 0.0f;
    size_t cnt = 0;
    float theta = M_PI*0.05f;
    while (cnt < 1000)
    {
        beg_time = std::chrono::steady_clock::now();
        
        ++cnt;
        float phi = float(cnt)*M_PI/180.0f;
        scene.camera.dir = SVector{ std::cos(theta)*std::cos(phi), 
                                    std::sin(theta),
                                    std::cos(theta)*std::sin(phi) };
        scene.camera.pos = SVector{ 0.0f, 0.0f, 1.0f } + 
                           scene.camera.dir*10.0f;
        scene.camera.up  = SVector{ 0.0f, 1.0f, 0.0f };
        
        SMatrixExt camera_mtx = scene.camera.get_matrix();
        SMatrixExt viewport_mtx = view_mtx*scene.projection.get_matrix();

        auto vert_shader_plane = 
            [&scene, &camera_mtx, &viewport_mtx]
        (SVertex& vertex)
        {
            vertex.point  = camera_mtx*vertex.point; 
            vertex.normal = ::normal(camera_mtx*vertex.normal); 
            vertex.color  = scene.light.apply(vertex, SVector{});
            vertex.point  = viewport_mtx*vertex.point; 
        };

        float scale_sky = 20.0f;
        auto vert_shader_sky = 
            [&scene, &scale_sky, &camera_mtx, &viewport_mtx]
        (SVertex& vertex)
        {
            vertex.point  = camera_mtx*(vertex.point*scale_sky); 
            vertex.normal = ::normal(camera_mtx*vertex.normal); 
            //vertex.color  = vertex.color;
            vertex.point  = viewport_mtx*vertex.point; 
        };

        pipeline.render_scene(object_sky.vertex_buf(),
                              object_sky.index_buf(), 
                              vert_shader_sky, frag_shader_sky, 
                              cull_face_sky);

        pipeline.render_scene(object_plane.vertex_buf(),
                              object_plane.index_buf(), 
                              vert_shader_plane, frag_shader_plane, 
                              cull_face_plane);

        pipeline.flush_screen(screen);
        pipeline.clear_buffer();

        seconds_elapsed = std::chrono::steady_clock::now() - beg_time;
        sum_fps += 1.0/seconds_elapsed.count();
    }

    printf("average fps: %lg", sum_fps/double(cnt));
    
    SGL_CLOSE_RESOURCE_FILE(obj_file_plane);
    SGL_CLOSE_RESOURCE_FILE(tex_file_plane);
    SGL_CLOSE_RESOURCE_FILE(obj_file_sky);
    SGL_CLOSE_RESOURCE_FILE(tex_file_sky);

    return 0;
}

#undef SGL_INIT_RESOURCE_FILE
#undef SGL_CLOSE_RESOURCE_FILE
