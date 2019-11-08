#ifndef SGL_SVERTEXSHADER_H
#define SGL_SVERTEXSHADER_H

//double including all tested files to check collisions
#include "SVertex.h"
#include "SScene.h"

//namespace sgl {

struct SVertexShader
{
public:
    explicit SVertexShader(const SScene& scene);
    void operator () (SVertex& vertex) const;
    
    SMatrixExt modelview_mtx; 
    SMatrixExt projection_mtx; 
    SLight     light;
    SVector    eyepos;
};

SVertexShader::SVertexShader(const SScene& scene)
{
    float side = fmin(scene.dim_x, scene.dim_y);
    
    float xx = 0.5f*side;  
    float yy = -0.5f*side; 

    float xw = 0.5f*scene.dim_x; 
    float yw = 0.5f*scene.dim_y; 

    SMatrixExt buf_mtx = SMatrixExt(SVectorExt{ xx,   0.0f, 0.0f, xw },
                                    SVectorExt{ 0.0f, yy,   0.0f, yw },
                                    SVectorExt{ 0.0f, 0.0f, 1.0f, 0.0f});

    modelview_mtx  = scene.camera.get_matrix()*scene.matrix; 
    projection_mtx = buf_mtx*scene.projection.get_matrix(); 

    light  = scene.light;
    eyepos = SVector{};//scene.camera.pos;
}

void SVertexShader::operator () (SVertex& vertex) const
{
    vertex.point  = modelview_mtx*vertex.point; 
    vertex.normal = ::normal(modelview_mtx*vertex.normal); 
    vertex.color  = light.apply(vertex, eyepos);
    vertex.point  = projection_mtx*vertex.point; 
}

//} //namespace sgl

#endif //SGL_SVERTEXSHADER_H
