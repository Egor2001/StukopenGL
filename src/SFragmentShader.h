#ifndef SGL_SFRAGMENTSHADER_H
#define SGL_SFRAGMENTSHADER_H

#include "SFragment.h"
#include "SScene.h"

//namespace sgl {

struct SFragmentShader
{
//no constructors to make type trivial
    void init(const SScene& scene, 
              float max_x, float max_y);

    template<typename TContainer>
    void apply(SFragment& fragment, 
               const TContainer& vert_container) const;
};

void SFragmentShader::init(const SScene& scene, 
                           float max_x, float max_y)
{}

template<typename TContainer>
void SFragmentShader::apply(SFragment& fragment, 
                            const TContainer& vert_container) const
{}

//} //namespace sgl

#endif //SGL_SFRAGMENTSHADER_H
