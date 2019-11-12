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
#include "src/CPerspective.h"
#include "src/CPipeline.h"
#include "src/CParallelPipeline.h"
#include "src/CTexture.h"
#include "src/CCullFace.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s OBJFILE TEXFILE \n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* obj_file = fopen(argv[1], "r+");
    FILE* tex_file = fopen(argv[2], "r+");

    if (!obj_file)
    {
        perror("error opening .obj file: ");
        return EXIT_FAILURE;
    }

    SScene scene = 
    {
        .dim_x = float(CBuffer::DIM_W),
        .dim_y = float(CBuffer::DIM_H),

        .matrix = SMatrixExt(),

        .camera = SCamera
        {
            .pos = SVector{ 0.0f, 1.0f, 3.0f },
//            .pos = SVector{ 0.0f, 0.0f, 0.0f },
            .dir = SVector{ 0.0f, 1.0f, 3.0f },
            .up  = SVector{ 0.0f, 1.0f, 0.0f }
        },

        .light = SLight
        { 
            .point = SVector{ -2.5f, -1.0f, 0.0f },
            .color = SColor{ 1.0f, 1.0f, 1.0f },
            .phong_ads = SVector{ 0.125f, 0.250f, 0.625f },
            //.phong_ads = SVector{ 0.0f, 1.0f, 0.0f },
            .phong_pow = 16.0f
        },

        .projection = CPerspective(1.0f, 16.0f),
        .texture = CTexture(tex_file)
    };

    CObject object;
    object.parse_from(obj_file, SColor{ 1.0f, 1.0f, 1.0f });

    CScreen screen = CScreen();
    
    auto cull_face_side = CCullFace<SGL_CULL_BACK>();

    auto pipeline = 
        CPipeline<CFillRasterizer>(float(CBuffer::DIM_W), 
                                   float(CBuffer::DIM_H));

    auto beg_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> seconds_elapsed;

    float scale = 1.0f;
    SMatrixExt scale_mtx = SMatrixExt(SVectorExt{ scale, 0.0f, 0.0f, 0.0f },
                                      SVectorExt{ 0.0f, scale, 0.0f, 0.0f },
                                      SVectorExt{ 0.0f, 0.0f, scale, 0.0f });

    double sum_fps = 0.0f;
    size_t cnt = 0;
    while (cnt < 1000)
    {
        beg_time = std::chrono::steady_clock::now();

        auto vert_shader = SVertexShader(scene);
        auto frag_shader = [&scene](SFragment& frag, const auto& vert_buf)
        {
            frag.color *= 
                scene.texture.get_color(frag.tex_u, 1.0f - frag.tex_v);
        };

        auto cull_face = 
            [&cull_face_side]
            (const SVertex& beg_v, const SVertex& mid_v, const SVertex& end_v) 
             -> bool
            {
                return (/*beg_v.point.z > 2.0f ||*/ beg_v.point.z < -2.0f || 
                        /*mid_v.point.z > 2.0f ||*/ mid_v.point.z < -2.0f ||
                        /*end_v.point.z > 2.0f ||*/ end_v.point.z < -2.0f) ||
                        cull_face_side(beg_v, mid_v, end_v);
            };
        
        ++cnt;
        float ang = float(cnt)*M_PI/180.0f;
        scene.matrix = scale_mtx * 
            SMatrixExt(SVectorExt(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                       SVectorExt(0.0f,      1.0f,       0.0f, 0.0f),
                       SVectorExt(sinf(ang), 0.0f,  cosf(ang), 0.0f));

        pipeline.render_scene(object, vert_shader, frag_shader, cull_face);

        pipeline.flush_screen(screen);
        pipeline.clear_buffer();

        seconds_elapsed = std::chrono::steady_clock::now() - beg_time;
        sum_fps += 1.0/seconds_elapsed.count();
    }

    printf("average fps: %lg", sum_fps/double(cnt));
    
    if (tex_file)
    {
        fclose(tex_file);
        tex_file = nullptr;
    }

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

