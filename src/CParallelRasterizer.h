#ifndef SGL_CPARALLELRASTERIZER_H
#define SGL_CPARALLELRASTERIZER_H

#include "parallel/CThreadPool.h"
#include "memory/CIntrinsicAllocator.h"
#include "CRasterizer.h"

//namespace sgl {

class CParallelRasterizer
{
public:
    static const size_t THREAD_CNT = 8;

    using vert_shader_t = SVertexSheader;
    //using frag_shader_t = SFragmentSheader;

    CParallelRasterizer();

private:
    CThreadPool                 thread_pool_;
    CIntrinsicVector<SFragment> frag_vec_arr_[THREAD_CNT];
};

//} //namespace sgl

#endif //SGL_CPARALLELRASTERIZER_H
