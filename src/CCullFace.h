#ifndef SGL_CCULLFACE_H 
#define SGL_CCULLFACE_H 

#include "SVertex.h"

//namespace sgl {

enum ECullType
{
    SGL_CULL_BACK,
    SGL_CULL_FRONT,
    SGL_CULL_DISABLED
};

template<ECullType CULL_TYPE>
class CCullFace
{
public:
    CCullFace() = default;

    bool operator () (const SVertex& beg_v, 
                      const SVertex& mid_v, 
                      const SVertex& end_v) const
    {
        bool result = false;

        //implemented this way to avoid extra computations in case of disable
        if constexpr (CULL_TYPE != ECullType::SGL_CULL_DISABLED)
        {
            float area = (beg_v.point.x*mid_v.point.w - 
                          mid_v.point.x*beg_v.point.w)*
                         (mid_v.point.y*end_v.point.w - 
                          end_v.point.y*mid_v.point.w) -  
                         (beg_v.point.y*mid_v.point.w - 
                          mid_v.point.y*beg_v.point.w)*
                         (mid_v.point.x*end_v.point.w - 
                          end_v.point.x*mid_v.point.w); 

            if constexpr (CULL_TYPE == ECullType::SGL_CULL_BACK)
                result = (area < 0.0f);
            else if constexpr (CULL_TYPE == ECullType::SGL_CULL_FRONT)
                result = (area > 0.0f);
        }

        return result; 
    }
};

//} //namespace sgl

#endif //SGL_CCULLFACE_H 
