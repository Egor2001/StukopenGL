//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <random>
#include <ctime>

//double including all tested files to check collisions
#include "src/SVector.h"
#include "src/SVector.h"
#include "src/SMatrix.h"
#include "src/SMatrix.h"
#include "src/SColor.h"
#include "src/SColor.h" 
#include "src/CRasterizer.h"
#include "src/CRasterizer.h"
#include "src/CScreen.h"
#include "src/CScreen.h"
#include "src/CBuffer.h"
#include "src/CBuffer.h"
#include "src/CObject.h"
#include "src/CObject.h"
#include "src/CCamera.h"
#include "src/CCamera.h"

int main(int argc, char* argv[])
{
    if (argc == 1 || argc > 2)
    {
        printf("Usage: %s OBJFILE \n", argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    FILE* obj_file = fopen(argv[1], "r+");

    if (!obj_file)
    {
        perror("error opening .obj file: ");
        return EXIT_FAILURE;
    }

    CObject obj = CObject();
    obj.parse_from(obj_file);

    CCamera cam = CCamera(SVector(0.75f, 1.25f, 1.0f), 
                          SVector(-0.75f, -1.25f, -1.0f));

    SMatrix mtx = cam.get_matrix();

    obj.write_to(stdout);

    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    CRasterizer rasterizer = CRasterizer();

    std::vector<SVertex> vert_buf = obj.vertex_buf();
    for (auto& vert : vert_buf)
    {
        vert.color = SColor(float(rand()&0xFF)/255.0f,
                            float(rand()&0xFF)/255.0f,
                            float(rand()&0xFF)/255.0f); 

        vert.point = mtx*vert.point;

        vert.point.x = 0.5f*float(CBuffer::DIM_H)*(1.0f + vert.point.x);
        vert.point.y = 0.5f*float(CBuffer::DIM_H)*(1.0f - vert.point.y);
    }

    for (auto& vert : vert_buf)
        printf("color: %.3f %.3f %.3f %.3f \n",
                vert.color.r, vert.color.g, vert.color.b,
                vert.color.a);

    for (auto& vert : vert_buf)
        printf("vector: %.3f %.3f %.3f %.3f \n",
                vert.point.x, vert.point.y, vert.point.z,
                vert.point.w);

    for (const auto& face : obj.index_buf())
    {
        rasterizer.rast_face(vert_buf[face.arr[0]],
                             vert_buf[face.arr[1]],
                             vert_buf[face.arr[2]]);  
    } 

    buf.render(rasterizer.frag_vec());
    scr.write(buf.data(), buf.byte_size());

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

