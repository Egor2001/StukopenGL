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
#include "src/CCamera.h"
#include "src/CVertexShader.h"
#include "src/CLight.h"
#include "src/CPerspective.h"
#include "src/CPipeline.h"

int test_pipeline(const SScene& scene)
{
    CPerspective projection = CPerspective(1.0f, 2.5f);
    CRasterizer  rasterizer = CRasterizer(float(CBuffer::DIM_W), 
                                          float(CBuffer::DIM_H));
    CScreen screen = CScreen();

    CPipeline pipeline = CPipeline(std::move(rasterizer), 
                                   std::move(projection));
    pipeline.render_scene(scene);
    pipeline.flush_screen(screen);
    //pipeline.clear_buffer();

    return 0;
}

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

    float ang = M_PI/4.0f;
    SScene scene = 
    {
        .object = CObject(),

        .matrix = SMatrixExt(SVectorExt(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                             SVectorExt(0.0f,      1.0f,       0.0f, 0.0f),
                             SVectorExt(sinf(ang), 0.0f,  cosf(ang), 0.0f)),

        .camera = CCamera(SVector{ 0.0f, 1.0f, 2.0f },
                          SVector{ 0.0f, 0.0f, -2.0f }),

        .light  = CLight(SVectorExt(5.0f, 5.0f, -5.0f, 1.0f),
                         SColor{ 1.0f, 1.0f, 1.0f })
    };

    scene.object.parse_from(obj_file);
    test_pipeline(scene);

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

