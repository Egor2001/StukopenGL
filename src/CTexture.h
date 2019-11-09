#ifndef SGL_CTEXTURE_H
#define SGL_CTEXTURE_H

#include <cstdio>
#include "math/SColorExt.h"

//testing
#include "CScreen.h"

//namespace sgl {

struct CTexture
{
public:
    const size_t CELL_SIZE = 16;

    struct STexColor
    {
        uint8_t r, g, b;
    };

    explicit CTexture(FILE* ppm_file);

    CTexture             (const CTexture&) = delete;
    CTexture& operator = (const CTexture&) = delete;

    CTexture             (CTexture&&);
    CTexture& operator = (CTexture&&);

    ~CTexture();

    size_t           byte_size() const noexcept;
    const STexColor* data()      const noexcept;

    SColor get_color(float tex_u, float tex_v) const noexcept;

private:
    void cleanup_() noexcept;

    size_t dim_w_;
    size_t dim_h_;

    STexColor* data_;
};

CTexture::CTexture(FILE* ppm_file):
    dim_w_(0), 
    dim_h_(0),
    data_(nullptr)
{
//is implemented in C-way to make constructor noexcept and safe
#define CHECK_PPM_PARSE(COND) \
    /* printf("%s\n", #COND); */ \
    if (!(COND)) goto error;
    
    int res = 0;
    size_t read = 0;

    CHECK_PPM_PARSE(ppm_file)

    res = fscanf(ppm_file, "P6%zn%*[ \n\r\t]", &read);
    CHECK_PPM_PARSE(res == 0 && read == sizeof("P6")-1)
    
    while (read)
    {
        read = 0;
        res = fscanf(ppm_file, "#%zn%*[^\r\n]%*[ \t\r\n]", &read);
        CHECK_PPM_PARSE(res == 0)
    }

    res = fscanf(ppm_file, "%zu %zu %*[ \n\r\t]", &dim_w_, &dim_h_);
    CHECK_PPM_PARSE(res == 2)

    res = fscanf(ppm_file, "255%zn%*[ \n\r\t]", &read);
    CHECK_PPM_PARSE(res == 0 && read == sizeof("255")-1)

    data_ = static_cast<STexColor*>(calloc(sizeof(STexColor), dim_w_*dim_h_));
    CHECK_PPM_PARSE(data_)

    read = fread(data_, sizeof(STexColor), dim_w_*dim_h_, ppm_file);
    CHECK_PPM_PARSE(read == dim_w_*dim_h_)

#undef CHECK_ERROR

    return;

//reachable only if error occured 
error:
    cleanup_();
}

CTexture::CTexture(CTexture&& move_texture):
    dim_w_(std::move(move_texture.dim_w_)), 
    dim_h_(std::move(move_texture.dim_h_)),
    data_(std::move(move_texture.data_))
{
    move_texture.dim_w_ = 0; 
    move_texture.dim_h_ = 0;
    move_texture.data_ = nullptr;
}

CTexture& CTexture::operator = (CTexture&& move_texture)
{
    std::swap(dim_w_, move_texture.dim_w_); 
    std::swap(dim_h_, move_texture.dim_h_);
    std::swap(data_, move_texture.data_);

    return *this;
}

CTexture::~CTexture()
{
    cleanup_();
}

size_t CTexture::byte_size() const noexcept
{
    return dim_w_*dim_h_*sizeof(SColor);
}

const CTexture::STexColor* 
CTexture::data() const noexcept
{
    return data_;
}

//TODO: to optimize it 
SColor CTexture::get_color(float tex_u, float tex_v) const noexcept
{
    size_t u = size_t(float(dim_w_)*fmax(0.0f, fmin(tex_u, 1.0f)));
    size_t v = size_t(float(dim_h_)*fmax(0.0f, fmin(tex_v, 1.0f)));
    size_t idx = dim_w_*v + u;

    return SColor{ float(data_[idx].b)/255,
                   float(data_[idx].g)/255, 
                   float(data_[idx].r)/255 };
}

void CTexture::cleanup_() noexcept
{
    if (data_)
    {
        free(data_); 
        data_ = nullptr;
    }

    dim_w_ = dim_h_ = 0;
}

int test_CTexture(int argc, char* argv[])
{
    if (argc == 1 || argc > 2)
    {
        printf("Usage: %s PPMFILE \n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* ppm_file = fopen(argv[1], "r+");

    if (!ppm_file)
    {
        perror("error opening .ppm file: ");
        return EXIT_FAILURE;
    }

    CScreen screen;
    CTexture texture(ppm_file);

    if (texture.data())
        screen.write(texture.data(), texture.byte_size());

    if (ppm_file)
    {
        fclose(ppm_file);
        ppm_file = nullptr;
    }

    return 0;
}

//} //namespace sgl

#endif //SGL_CTEXTURE_H
