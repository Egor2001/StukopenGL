#ifndef SGL_SCREEN_H
#define SGL_SCREEN_H

//general
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <cstdint>

//screen
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//namespace sgl {

class CScreen
{
public:
    explicit CScreen(const char* fb_file_name = "/dev/fb0"):
        fb_file_(open(fb_file_name, O_RDWR))
    {}

    ~CScreen()
    {
        if (fb_file_ != -1)
        {
            close(fb_file_); 
            fb_file_ = -1;
        }
    }

    int write(const void* src, size_t cnt)    
    {
        int result = ::write(fb_file_, src, cnt);
        return result;
    }

private:
    int fb_file_;
};

int test_CScreen() 
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

//} //namespace sgl 

#endif //SGL_SCREEN_H

