//general
#include <cstdio>
#include <cstdlib>
#include <cstdint>

//double including all tested files to check collisions
#include "SVector.h" 
#include "SVector.h" 
#include "CScreen.h"
#include "CScreen.h"
#include "CBuffer.h"
#include "CBuffer.h"

int integration_CScreen_CBuffer()
{
    CScreen scr = CScreen();
    CBuffer buf = CBuffer();

    auto frag_vec = std::vector<CBuffer::SBufFragment>(0x100000);
    for (size_t i = 0; i < frag_vec.size(); ++i)
    {
        frag_vec[i].x = rand()%CBuffer::DIM_W;
        frag_vec[i].y = rand()%CBuffer::DIM_H;

        frag_vec[i].color.r = rand()&0xFF;
        frag_vec[i].color.g = rand()&0xFF;
        frag_vec[i].color.b = rand()&0xFF;
        frag_vec[i].color.a = rand()&0xFF;
    } 

    buf.render(frag_vec);
    scr.write(reinterpret_cast<const uint8_t*>(buf.data()), buf.byte_size());

    return 0;
}

int unit_CBuffer()
{
    CBuffer buf = CBuffer();

    const size_t SIZE = buf.size();
    auto frag_vec = std::vector<CBuffer::SBufFragment>(SIZE);

    for (size_t i = 0; i < SIZE; ++i)
    {
        frag_vec[i].x = rand()%CBuffer::DIM_W;
        frag_vec[i].y = rand()%CBuffer::DIM_H;

        frag_vec[i].color.r = rand()%256;    
        frag_vec[i].color.g = rand()%256;    
        frag_vec[i].color.b = rand()%256;    
        frag_vec[i].color.a = rand()%256;    
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

int unit_CScreen() 
{
    CScreen screen = CScreen(); 

    const size_t SIZE = 0x100000;
    uint8_t* buf = static_cast<uint8_t*>(malloc(SIZE));

    int rand_fd = open("/dev/urandom", O_RDONLY);
    
    read(rand_fd, buf, SIZE);

    if (rand_fd != -1)
        close(rand_fd);
    
    rand_fd = -1;
    
    screen.write(buf, SIZE);
    
    if (buf)
        free(buf);

    return 0;
}

int unit_SVector()
{
    SVector vec1 = SVector(0.5f, 6.7f, -3.4f);
    SVector vec2 = SVector(-4.3f, 0.1f, 2.6f, 0.5f);

    SVector add = vec1 + vec2;
    SVector sub = vec1 - vec2;
    SVector mul_vec = vector_mul(vec1, vec2);
    float   mul_scal = scalar_mul(vec1, vec2);

    printf("vec1    %f %f %f %f\n", vec1.x, vec1.y, vec1.z, vec1.w);
    printf("vec2    %f %f %f %f\n", vec2.x, vec2.y, vec2.z, vec2.w);
    printf("add     %f %f %f %f\n", add.x, add.y, add.z, add.w);
    printf("sub     %f %f %f %f\n", sub.x, sub.y, sub.z, sub.w);
    printf("mul_vec %f %f %f %f\n", mul_vec.x, mul_vec.y, mul_vec.z, mul_vec.w);
    printf("mul_scal %f\n", mul_scal);

    return 0;
}

//} //namespace sgl

