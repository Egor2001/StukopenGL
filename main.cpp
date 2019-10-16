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
#include "src/CVertexShader.h"
#include "src/CVertexShader.h"

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

    CObject obj = CObject();
    obj.parse_from(obj_file);
    obj.write_to(stdout);

    CCamera cam = CCamera(SVector(0.75f, 1.25f, 1.0f), 
                          SVector(-0.75f, -1.25f, -1.0f));

    float x_scale =  0.5f*float(CBuffer::DIM_H);
    float y_scale = -0.5f*float(CBuffer::DIM_H);

    float x_shift = 0.5f*float(CBuffer::DIM_W);
    float y_shift = 0.5f*float(CBuffer::DIM_H);

    SMatrix cam_mtx = cam.get_matrix();
    SMatrix scr_mtx = SMatrix(SVector(x_scale, 0.0f, 0.0f, x_shift), 
                              SVector(0.0f, y_scale, 0.0f, y_shift), 
                              SVector(0.0f, 0.0f, 1.0f, 0.0f)); 

    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    CRasterizer rasterizer = CRasterizer();

    auto seed = time(NULL);

    std::vector<SVertex> vert_buf = obj.vertex_buf();
    for (auto& vert : vert_buf)
    {
        srand(seed ^ int(vert.point.x*4000.0f) ^
                     int(vert.point.y*2000.0f) ^ 
                     int(vert.point.z*1000.0f));

        vert.color = SColor(float(rand()&0xFF)/255.0f,
                            float(rand()&0xFF)/255.0f,
                            float(rand()&0xFF)/255.0f); 
    }

    for (auto& vert : vert_buf)
        printf("color: %.3f %.3f %.3f %.3f \n",
                vert.color.r, vert.color.g, vert.color.b,
                vert.color.a);

    for (auto& vert : vert_buf)
        printf("vector: %.3f %.3f %.3f %.3f \n",
                vert.point.x, vert.point.y, vert.point.z,
                vert.point.w);

    buf.clear();
    rasterizer.clear();
 
    SMatrix cur_mtx = SMatrix();
    while(true)
    {
        cur_mtx *= SMatrix(SVector(sqrtf(3.0f)*0.5f, 0.0f, -0.5f, 0.0f),
                           SVector(0.0f, 1.0f, 0.0f, 0.0f),
                           SVector(0.5f, 0.0f,  sqrtf(3.0f)*0.5f, 0.0f));

        CVertexShader vertex_shader = CVertexShader(scr_mtx*cam_mtx*cur_mtx);
        
        SVertex vert_arr[3] = {};
        for (const auto& face : obj.index_buf())
        {
            for (size_t i = 0; i < 3; ++i)
                vert_arr[i] = vertex_shader.apply(vert_buf[face.arr[i]]);

            rasterizer.fill_face(vert_arr);
        } 

        buf.render(rasterizer.frag_vec());
        scr.write(buf.data(), buf.byte_size());
        buf.clear();
        rasterizer.clear();
    }

    if (obj_file)
    {
        fclose(obj_file);
        obj_file = nullptr;
    }

    return 0;
}

