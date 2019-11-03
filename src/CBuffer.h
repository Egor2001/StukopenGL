#ifndef SGL_CBUFFER_H
#define SGL_CBUFFER_H

//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//buffer
#include <cstring>
#include <wchar.h>
#include <cfloat>
#include <vector>
#include "math/SMatrixExt.h"
#include "ShaderStructures.h"
#include "memory/CIntrinsicAllocator.h"

//test
#include <random>

//namespace sgl {

class CBuffer
{
public:
    static const size_t DIM_W = 1376;
    static const size_t DIM_H = 768;

    struct SBufColor
    {
        uint8_t r, g, b, a;
    };

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

    SMatrixExt get_matrix() const
    {
        float side = float(std::min(DIM_W, DIM_H));

        float x_scale =  0.5f*side;
        float y_scale = -0.5f*side;

        float x_shift = 0.5*float(DIM_W);
        float y_shift = 0.5*float(DIM_H);

        return SMatrixExt(SVectorExt(x_scale, 0.0f, 0.0f, x_shift),
                          SVectorExt(0.0f, y_scale, 0.0f, y_shift),
                          SVectorExt(0.0f, 0.0f, 1.0f, 0.0f));
    }

    void render(const CIntrinsicVector<SFragment>& fragment_vec)
    {
        for (const auto& fragment : fragment_vec)
            render_frag(fragment);
    }

    void render_frag(const SFragment& fragment)
    {
        size_t x = size_t(roundf(fragment.point.x));
        size_t y = size_t(roundf(fragment.point.y));

        size_t idx = DIM_W*y + x; 

        if (x < DIM_W && y < DIM_H &&
            fragment.point.z < depth_buf_[idx])
        {
            color_buf_[idx].r = uint8_t(255.0f*fragment.color.r);
            color_buf_[idx].g = uint8_t(255.0f*fragment.color.g);
            color_buf_[idx].b = uint8_t(255.0f*fragment.color.b);
            color_buf_[idx].a = 255;

            depth_buf_[idx] = fragment.point.z;
        }
    }

    void clear(const SBufColor& color_val = SBufColor(), 
               const float depth_val = FLT_MAX)
    {
        wchar_t color_wchar = 0;
        wchar_t depth_wchar = 0;
        memcpy(&color_wchar, &color_val, sizeof(wchar_t));
        memcpy(&depth_wchar, &depth_val, sizeof(wchar_t));

        wmemset((wchar_t*)color_buf_, color_wchar, DIM_W*DIM_H);
        wmemset((wchar_t*)depth_buf_, depth_wchar, DIM_W*DIM_H);
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

    auto fragment_vec = CIntrinsicVector<SFragment>(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        fragment_vec[i].point.x = float(rand()%CBuffer::DIM_W);
        fragment_vec[i].point.y = float(rand()%CBuffer::DIM_H);

        fragment_vec[i].color.r = float(rand()&0xFF)/255.0f;    
        fragment_vec[i].color.g = float(rand()&0xFF)/255.0f;    
        fragment_vec[i].color.b = float(rand()&0xFF)/255.0f;    
    }

    buf.render(fragment_vec);

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

