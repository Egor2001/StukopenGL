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
#include "src/CLight.h"
#include "src/CLight.h"

class CPerspective
{
public:
    CPerspective(float near_set, float far_set);

    CPerspective             (const CPerspective&);
    CPerspective& operator = (const CPerspective&);
    CPerspective             (CPerspective&&);
    CPerspective& operator = (CPerspective&&);

    ~CPerspective();

    SMatrix get_matrix() const;

private:
    float near_;
    float far_;
};

CPerspective::CPerspective(float near_set, float far_set):
    near_(near_set),
    far_ (far_set)
{}

CPerspective::CPerspective(const CPerspective& copy_perspective):
    near_(copy_perspective.near_),
    far_ (copy_perspective.far_)
{}

CPerspective& CPerspective::operator = (const CPerspective& copy_perspective)
{
    near_ = copy_perspective.near_;
    far_  = copy_perspective.far_;

    return *this;
}

CPerspective::CPerspective(CPerspective&& move_perspective):
    near_(std::move(move_perspective.near_)),
    far_ (std::move(move_perspective.far_))
{}

CPerspective& CPerspective::operator = (CPerspective&& move_perspective)
{
    std::swap(near_, move_perspective.near_);
    std::swap(far_,  move_perspective.far_);

    return *this;
}

CPerspective::~CPerspective()
{
    near_ = far_ = 0.0f;
}

SMatrix CPerspective::get_matrix() const
{
    float mat33 = -(far_ + near_)/(far_ - near_);
    float mat34 = -(2*far_*near_)/(far_ - near_);

    SMatrix result = SMatrix(SVector(near_, 0.0f, 0.0f, 0.0f),
                             SVector(0.0f, near_, 0.0f, 0.0f),
                             SVector(0.0f, 0.0f, mat33, mat34),
                             SVector(0.0f, 0.0f, -1.0f, 0.0f));

    return result;
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

    CObject obj = CObject();
    obj.parse_from(obj_file);
    //obj.write_to(stdout);

    SVector cam_pos = SVector(0.0f, 1.0f, 2.0f);
    SVector cam_dir = SVector(0.0f, 0.0f, -2.0f);

    CCamera cam   = CCamera(cam_pos, cam_dir);
    CLight  light = CLight (SVector(1.0f, 1.0f, -1.0f, 0.0f), 
                            SColor(1.0f, 1.0f, 1.0f, 1.0f));
    CPerspective proj = CPerspective(0.1f, 10.0f);

    float x_scale =  0.5f*float(CBuffer::DIM_H);
    float y_scale = -0.5f*float(CBuffer::DIM_H);

    float x_shift = 0.5f*float(CBuffer::DIM_W);
    float y_shift = 0.5f*float(CBuffer::DIM_H);

    SMatrix cam_mtx = cam.get_matrix();
    SMatrix buf_mtx = SMatrix(SVector(x_scale, 0.0f, 0.0f, x_shift), 
                              SVector(0.0f, y_scale, 0.0f, y_shift), 
                              SVector(0.0f, 0.0f, 1.0f, 0.0f)); 
    SMatrix proj_mtx = proj.get_matrix();

    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    CRasterizer rasterizer = CRasterizer(float(CBuffer::DIM_W), 
                                         float(CBuffer::DIM_H));

//    auto seed = time(NULL);
//    buf.clear();
    rasterizer.clear();
 
    SMatrix cur_mtx = SMatrix();
    float delta = M_PI/16.0f;
    float ang = 0.0f;
    while(true)
    {
        ang += delta;
        cur_mtx = SMatrix(SVector(cosf(ang), 0.0f, -sinf(ang), 0.0f),
                          SVector(0.0f,      1.0f,       0.0f, 0.0f),
                          SVector(sinf(ang), 0.0f,  cosf(ang), 0.0f));

        CVertexShader vertex_shader = CVertexShader(buf_mtx*cam_mtx*cur_mtx);
        
        SVertex vert_arr[3] = {};
        for (const auto& face : obj.index_buf())
        {
            for (size_t i = 0; i < 3; ++i)
            {
                vert_arr[i] = obj.vertex_buf()[face.arr[i]]; 
                vert_arr[i].color = SColor(0.5f, 0.5f, 0.5f);
                vert_arr[i] = light.apply(vertex_shader.apply(vert_arr[i]));
            }

            rasterizer.fill_face(vert_arr);
//            rasterizer.rast_face(vert_arr[0], vert_arr[1], vert_arr[2]);
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

