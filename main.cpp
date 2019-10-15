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

class CVertexShader
{
public:
    using in_t  = SVertex;
    using out_t = SVertex;

    CVertexShader();
    explicit CVertexShader(const SMatrix& mtx_set);

    CVertexShader             (const CVertexShader&);
    CVertexShader& operator = (const CVertexShader&);

    CVertexShader             (CVertexShader&&);
    CVertexShader& operator = (CVertexShader&&);

    ~CVertexShader();

    out_t apply(const in_t& in) const;
    
private:
    SMatrix mtx_; 
};

CVertexShader::CVertexShader():
    mtx_()
{}

CVertexShader::CVertexShader(const SMatrix& mtx_set):
    mtx_(mtx_set)
{}

CVertexShader::CVertexShader(const CVertexShader& copy_shader):
    mtx_(copy_shader.mtx_)
{}

CVertexShader& CVertexShader::operator = (const CVertexShader& copy_shader)
{
    mtx_ = copy_shader.mtx_;

    return *this;
}

CVertexShader::CVertexShader(CVertexShader&& move_shader):
    mtx_(std::move(move_shader.mtx_))
{}

CVertexShader& CVertexShader::operator = (CVertexShader&& move_shader)
{
    std::swap(mtx_, move_shader.mtx_);

    return *this;
}

CVertexShader::~CVertexShader()
{
    mtx_ = SMatrix();
}

CVertexShader::out_t CVertexShader::apply(const in_t& in) const
{
    out_t result = out_t();

    result.point  = unitary(mtx_*in.point); 
    result.normal =              in.normal; 
    result.color  =              in.color;
 
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

    CVertexShader vertex_shader = CVertexShader(scr_mtx*cam_mtx);

    obj.write_to(stdout);

    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    CRasterizer rasterizer = CRasterizer();

    auto seed = time(NULL);

    std::vector<SVertex> vert_buf = obj.vertex_buf();
    for (auto& vert : vert_buf)
    {
        vert = vertex_shader.apply(vert);

        srand(seed ^ int(vert.point.x) ^
                     int(vert.point.y) ^ 
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

    SVertex vert_arr[3] = {};
    for (const auto& face : obj.index_buf())
    {
        for (size_t i = 0; i < 3; ++i)
            vert_arr[i] = vert_buf[face.arr[i]];

        rasterizer.fill_face(vert_arr);
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

