#ifndef SGL_SSCENE_H
#define SGL_SSCENE_H

#include "SCamera.h"
#include "SLight.h"
#include "SPerspective.h"

//namespace sgl {

//TODO: to add support for multiple lights
struct SScene
{
public:
    float        dim_x, dim_y;
    SCamera      camera;
    SLight       light;
    SPerspective projection;
};

//} //namespace sgl

#endif //SGL_SSCENE_H
