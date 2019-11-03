#ifndef SGL_SFRAGMENTSHADER_H
#define SGL_SFRAGMENTSHADER_H

#include "ShaderStructures.h"

//namespace sgl {

struct SFragmentShader
{
//no constructors to make type trivial
    using in_t = SFragment;
    using out_t = SFragment;

    SFragment apply(const SFragment& fragment);
};

typename SFragmentShader::out_t 
SFragmentShader::apply(const in_t& fragment)
{
    out_t result = fragment; 
    return result;
}

//} //namespace sgl

#endif //SGL_SFRAGMENTSHADER_H
