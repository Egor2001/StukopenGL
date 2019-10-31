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
using CIntrinsicVector = std::vector<T, CIntrinsicAllocator<T>>;

template<typename T>
class CIntrinsicAllocator
{
public:
    using value_type = T;
    static const size_t ALLOC_ALIGN = alignof(value_type);

    value_type* allocate(size_t n); 
    void        deallocate(value_type* ptr, size_t n); 
};

template<typename T>
typename CIntrinsicAllocator<T>::value_type* 
CIntrinsicAllocator<T>::allocate(size_t n) 
{
    return static_cast<value_type*>(_mm_malloc(n*sizeof(value_type), 
                                               ALLOC_ALIGN));
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

int test_CIntrinsicAllocator()
{
    const size_t VEC_SIZE = 255;
    CIntrinsicVector<__m256> vec;
    for (size_t i = 0; i < VEC_SIZE; ++i)
        vec.push_back(_mm256_set1_ps(1.0f));

    __m256 acc = _mm256_set1_ps(0.0f);
    for (auto& ymm : vec)
        acc = _mm256_add_ps(acc, ymm);

    printf("allocated at: %p\n", vec.data());

    for (size_t i = 0; i < 8; ++i)
        printf("acc[%llu] = %.3f ", i, acc[i]);

    printf("\n");

    return 0;
}

//} //namespace sgl

#endif //SGL_CINTRINSIC_ALLOCATOR_H
