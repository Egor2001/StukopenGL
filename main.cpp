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
#include "src/CScreen.h"
#include "src/CBuffer.h"
#include "src/CObject.h"
#include "src/SCamera.h"
#include "src/SVertexShader.h"
#include "src/SLight.h"
#include "src/CPerspective.h"
#include "src/CPipeline.h"

int main(int argc, char* argv[])
{
    if (argc == 1 || argc > 2)
    {
        printf("Usage: %s OBJFILE \n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* obj_file = fopen(argv[1], "r+");

    if (!obj_file)
    {
        perror("error opening .obj file: ");
        return EXIT_FAILURE;
    }

    SScene scene = 
    {
        .matrix = SMatrixExt(),

        .camera = SCamera
        {
            .pos = SVector{ 0.0f, 1.0f, 2.0f },
            .dir = SVector{ 0.0f, 0.0f, -2.0f },
            .up  = SVector{ 0.0f, 1.0f, 0.0f }
        },

        .light = SLight
        { 
            .point = SVector{ -2.5f, -1.0f, 0.0f },
            .color = SColor{ 1.0f, 1.0f, 0.0f },
            .phong_ads = SVector{ 0.125f, 0.250f, 0.625f },
            //.phong_ads = SVector{ 0.0f, 1.0f, 0.0f },
            .phong_pow = 16.0f
        },

        .projection = CPerspective(1.0f, 2.5f)
    };

    CObject object;
    object.parse_from(obj_file, SColor{ 1.0f, 1.0f, 1.0f });

    CScreen screen = CScreen();

    auto pipeline = 
        CPipeline<CFillRasterizer>(float(CBuffer::DIM_W), 
                                   float(CBuffer::DIM_H));

    auto beg_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> seconds_elapsed;

    double sum_fps = 0.0f;
    size_t cnt = 0;
    while (cnt < 1000)
    {
        beg_time = std::chrono::steady_clock::now();

        ++cnt;
        float ang = float(cnt)*M_PI/180.0f;
        scene.matrix = 
            SMatrixExt(SVectorExt(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                       SVectorExt(0.0f,      1.0f,       0.0f, 0.0f),
                       SVectorExt(sinf(ang), 0.0f,  cosf(ang), 0.0f));

        pipeline.render_scene(scene, object);
        pipeline.flush_screen(screen);
        pipeline.clear_buffer();

        seconds_elapsed = (beg_time - std::chrono::steady_clock::now());
        sum_fps += 1.0/seconds_elapsed.count();
    }

    printf("average fps: %lg", sum_fps/double(cnt));

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

