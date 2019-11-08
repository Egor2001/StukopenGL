#ifndef SGL_SFRAGMENTSHADER_H
#define SGL_SFRAGMENTSHADER_H

#include "SFragment.h"
#include "SScene.h"

//namespace sgl {

struct SFragmentShader
{
//no constructors to make type trivial
    explicit SFragmentShader(const SScene& scene);

    template<typename TContainer>
    void operator () (SFragment& fragment, 
                      const TContainer& vert_container) const;
};

SFragmentShader::SFragmentShader(const SScene& scene)
{}

template<typename TContainer>
void SFragmentShader::operator () (SFragment& fragment, 
                                   const TContainer& vert_container) const
{}

//} //namespace sgl

#endif //SGL_SFRAGMENTSHADER_H
