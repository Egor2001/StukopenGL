#ifndef SGL_SSCENE_H
#define SGL_SSCENE_H

#include "SCamera.h"
#include "SLight.h"
#include "CPerspective.h"

//namespace sgl {

//TODO: to add support for multiple lights
struct SScene
{
public:
    float        dim_x, dim_y;
    SCamera      camera;
    SLight       light;
    CPerspective projection;
};

//} //namespace sgl

#endif //SGL_SSCENE_H
