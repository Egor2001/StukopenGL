#ifndef SGL_SSCENE_H
#define SGL_SSCENE_H

#include "math/SMatrixExt.h"
#include "SCamera.h"
#include "SLight.h"
#include "CPerspective.h"
//#include "CTexture.h"

//namespace sgl {

//TODO: to add support for multiple lights
struct SScene
{
public:
    SMatrixExt   matrix;
    SCamera      camera;
    SLight       light;
    CPerspective projection;
    //CTexture texture;
};

//} //namespace sgl

#endif //SGL_SSCENE_H
