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
//    obj.write_to(stdout);

    SVector cam_pos = SVector{ 0.0f, 1.0f, 2.0f };
    SVector cam_dir = SVector{ 0.0f, 0.0f, -2.0f };

    CCamera cam   = CCamera(cam_pos, cam_dir);
    CLight  light = CLight (SVectorExt(5.0f, 5.0f, -5.0f, 1.0f), 
                            SColor{ 1.0f, 1.0f, 1.0f });
    CPerspective proj = CPerspective(1.0f, 2.5f);

    float x_scale =  0.5f*float(CBuffer::DIM_H);
    float y_scale = -0.5f*float(CBuffer::DIM_H);

    float x_shift = 0.5f*float(CBuffer::DIM_W);
    float y_shift = 0.5f*float(CBuffer::DIM_H);

    SMatrixExt cam_mtx = cam.get_matrix();
    SMatrixExt buf_mtx = SMatrixExt(SVectorExt(x_scale, 0.0f, 0.0f, x_shift), 
                                    SVectorExt(0.0f, y_scale, 0.0f, y_shift), 
                                    SVectorExt(0.0f, 0.0f, 1.0f, 0.0f)); 
    SMatrixExt proj_mtx = proj.get_matrix();

    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    CRasterizer rasterizer = CRasterizer(float(CBuffer::DIM_W), 
                                         float(CBuffer::DIM_H));

//    auto seed = time(NULL);
    buf.clear();
    rasterizer.clear();
 
    SMatrixExt cur_mtx = SMatrixExt();
    float delta = M_PI/180.0f;
    float ang = 0.0f;
    while(true)
    {
        ang += delta;
        cur_mtx = SMatrixExt(SVectorExt(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                             SVectorExt(0.0f,      1.0f,       0.0f, 0.0f),
                             SVectorExt(sinf(ang), 0.0f,  cosf(ang), 0.0f));

        CVertexShader vertex_shader = CVertexShader(cam_mtx*cur_mtx);
        
        SVertex vert_arr[3] = {};
        for (const auto& face : obj.index_buf())
        {
            for (size_t i = 0; i < 3; ++i)
            {
                vert_arr[i] = obj.vertex_buf()[face.arr[i]]; 
                vert_arr[i].color = SColor{ 0.5f, 0.5f, 0.5f };
                vert_arr[i] = light.apply(vertex_shader.apply(vert_arr[i]));
                vert_arr[i].point = buf_mtx*proj_mtx*vert_arr[i].point;
            }

            rasterizer.fill_face(vert_arr);
//            rasterizer.rast_face(vert_arr[0], vert_arr[1], vert_arr[2]);
        } 

//        printf("frag_vec size : %lu\n", rasterizer.frag_vec().size());
/*
        for (const auto& frag : rasterizer.frag_vec())
        {
            printf("frag: [point %.3f %.3f %.3f] "
                         "[color %.3f %.3f %.3f] \n", 
                   frag.point.x, frag.point.y, frag.point.z,
                   frag.color.r, frag.color.g, frag.color.b);
        }
*/
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

