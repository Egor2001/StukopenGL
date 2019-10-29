#ifndef SGL_CMEMORYPOOL_H
#define SGL_CMEMORYPOOL_H

#include <cstring>

//namespace sgl {

class CMemoryPool
{
public:
    struct SMemoryChunk
    {
    };

    CMemoryPool();

    CMemoryPool             (const CMemoryPool&) = delete;
    CMemoryPool& operator = (const CMemoryPool&) = delete;
    CMemoryPool             (CMemoryPool&&);
    CMemoryPool& operator = (CMemoryPool&&);

    ~CMemoryPool();

private:
    std::vector<SMemoryChunk> chunk_vec_;
};

//} //namespace sgl

#endif //SGL_CMEMORYPOOL_H
