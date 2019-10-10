#ifndef SGL_CBUFFER_H
#define SGL_CBUFFER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//buffer
#include <cfloat>
#include <vector>
#include "ShaderStructures.h"

//test
#include <random>

//namespace sgl {

class CBuffer
{
public:
    static const size_t DIM_W = 1376;
    static const size_t DIM_H = 768;

    using SBufFragment = SFragment;
    using SBufColor    = SFragment::SFragColor;

public:
    CBuffer();

    CBuffer             (const CBuffer&) = delete;
    CBuffer& operator = (const CBuffer&) = delete;

    CBuffer             (CBuffer&&);
    CBuffer& operator = (CBuffer&&);

    ~CBuffer();

    size_t size() const noexcept
    {
        return DIM_W*DIM_H;
    }

    size_t byte_size() const noexcept
    {
        return DIM_W*DIM_H*sizeof(SBufColor);
    }

    const SBufColor* data() const noexcept
    {
        return color_buf_;
    }

    void render(const std::vector<SBufFragment>& frag_vec)
    {
        for (const auto& frag : frag_vec)
            render_frag(frag);
    }

    void render_frag(const SBufFragment& frag)
    {
        size_t idx = DIM_W*frag.y + frag.x; 

        if (frag.x < DIM_W && frag.y < DIM_H &&
            frag.depth < depth_buf_[idx])
        {
            color_buf_[idx] = frag.color;
            depth_buf_[idx] = frag.depth;
        }
    }

    void clear()
    {
        for (size_t i = 0; i < DIM_W*DIM_H; ++i)
        {
            color_buf_[i] = {0xFF, 0xFF, 0xFF, 0xFF};
            depth_buf_[i] = FLT_MAX;
        }
    }

private:
    SBufColor* color_buf_;
    float*     depth_buf_;
};

CBuffer::CBuffer():
    color_buf_(static_cast<SBufColor*>(calloc(DIM_W*DIM_H, sizeof(SBufColor)))),
    depth_buf_(static_cast<float*>    (calloc(DIM_W*DIM_H, sizeof(float))))
{
    if (depth_buf_)
    {
        for (size_t i = 0; i < DIM_W*DIM_H; ++i)
            depth_buf_[i] = FLT_MAX;
    }
} 

CBuffer::CBuffer(CBuffer&& move_buffer):
    color_buf_(std::move(move_buffer.color_buf_)),
    depth_buf_(std::move(move_buffer.depth_buf_))
{}

CBuffer& CBuffer::operator = (CBuffer&& move_buffer)
{
    color_buf_ = std::move(move_buffer.color_buf_);
    depth_buf_ = std::move(move_buffer.depth_buf_);

    return *this;
}

CBuffer::~CBuffer()
{
    if (color_buf_)
    {
        free(color_buf_);
        color_buf_ = nullptr;
    }

    if (depth_buf_)
    {
        free(depth_buf_);
        depth_buf_ = nullptr;
    }
}

int test_CBuffer()
{
    CBuffer buf = CBuffer();

    const size_t SIZE = buf.size();
    auto frag_vec = std::vector<CBuffer::SBufFragment>(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        frag_vec[i].x = rand()%CBuffer::DIM_W;
        frag_vec[i].y = rand()%CBuffer::DIM_H;

        frag_vec[i].color.r = rand()&0xFF;    
        frag_vec[i].color.g = rand()&0xFF;    
        frag_vec[i].color.b = rand()&0xFF;    
        frag_vec[i].color.a = rand()&0xFF;    
    }

    buf.render(frag_vec);

    FILE* out_f = fopen("bitmap.bmp", "w");

    fwrite(buf.data(), sizeof(CBuffer::SBufColor), buf.size(), out_f);

    if (out_f)
    {
        fclose(out_f);
        out_f = nullptr;
    }

    return 0;
}

//} //namespace sgl

#endif //SGL_CBUFFER_H
