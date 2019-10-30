#ifndef SGL_CINTRINSIC_ALLOCATOR_H
#define SGL_CINTRINSIC_ALLOCATOR_H

//general
#include <immintrin.h>
#include <malloc.h>
#include <vector>

//testing
#include <cstdio>
#include <float.h>

//namespace sgl {

template<typename T>
class CIntrinsicAllocator;

template<typename T>
using CIntrinsicVector = std::vector<T, CIntrinsicAllocator>;

template<typename T>
class CIntrinsicAllocator
{
public:
    static const size_t ALLOC_ALIGN = 32;
    using value_type = T;

    value_type* allocate(size_t n); 
    void        deallocate(value_type* ptr, size_t n); 
};

template<typename T>
typename CIntrinsicAllocator<T>::value_type* 
CIntrinsicAllocator<T>::allocate(size_t n) 
{
    return static_cast<value_type*>(_mm_malloc(n, ALLOC_ALIGN));
}

template<typename T>
void 
CIntrinsicAllocator<T>::deallocate(value_type* ptr, size_t n) 
{
    _mm_free(static_cast<void*>(ptr));
}

int test_CIntrinsicAllocator()
{
    return 0;
}

//} //namespace sgl

#endif //SGL_CINTRINSIC_ALLOCATOR_H
