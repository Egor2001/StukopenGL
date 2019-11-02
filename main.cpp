//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions

#include "src/SVectorExt.h"
#include "src/SMatrixExt.h"
#include "src/SColorExt.h"
#include "src/CRasterizer.h"
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
        .object = CObject(),
        .matrix = SMatrixExt(),

        .camera = SCamera
        {
            .pos = SVector{ 0.0f, 1.0f, 2.0f },
            .dir = SVector{ 0.0f, 0.0f, -2.0f },
            .up  = SVector{ 0.0f, 1.0f, 0.0f }
        },

        .light = SLight
        { 
            .point = SVector{ 2.0f, 0.0f, 1.0f },
            .color = SColor{ 1.0f, 1.0f, 0.0f },
            //.phong_ads = SVector{ 0.25f, 0.5f, 0.25f },
            .phong_ads = SVector{ 0.0f, 0.0f, 1.0f },
            .phong_pow = 10.0f
        }
    };

    scene.object.parse_from(obj_file);

    CPerspective projection = CPerspective(1.0f, 2.5f);
    CRasterizer  rasterizer = CRasterizer(float(CBuffer::DIM_W), 
                                          float(CBuffer::DIM_H));
    CScreen screen = CScreen();

    CPipeline pipeline = CPipeline(std::move(rasterizer), 
                                   std::move(projection));

    size_t cnt = 0;
    while (true)
    {
        ++cnt;
        float ang = float(cnt)*M_PI/180.0f;
        scene.matrix = 
            SMatrixExt(SVectorExt(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                       SVectorExt(0.0f,      1.0f,       0.0f, 0.0f),
                       SVectorExt(sinf(ang), 0.0f,  cosf(ang), 0.0f));

        pipeline.render_scene(scene);
        pipeline.flush_screen(screen);
        pipeline.clear_buffer();
    }

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

